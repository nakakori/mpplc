/* ll_parse.c */

#include "ll_parse.h"

static int token;
static char errmes[MAXSTRSIZE];
static int bflag; // break flag (more 1: in loop, 0: not in loop)
static int pflag; // parameter flag (1: paramter, 0: not parameter)

static char *type_str[] = {"", "integer", "char", "boolean"};

static struct LABELS *loop_labels;
static ID *curvar; // current variable
static int type_curvar; // current variable type(LEFT_VAR or RIGHT_VAR)
static sbool trueflag;
static sbool falseflag;

static int block(char *label);
static int var_declare();
static int arrange_var();
// static int var_name(); // it might be omitted
static int type();
static int standard_type();
static int array_type();
static int sub_program();
// static int procedure_name(); // it might be omitted
static int parameter(UNDEFINE_LIST **list);
static int compound();
static int statement();
static int branch();
static int loop();
static int escape(); // it might be omitted
static int call_procedure();
static int arrange_expression();
static int back(); // it might be omitted
static int assign();
// static int left_hand();
static int var();
static int expression();
static int simple_expression();
static int term();
static int factor();
static int constant();
static int multi_operator(); // it might be omitted
static int add_operator(); // it might be omitted
static int relatinal_operator(); // it might be omitted
static int input();
static int output();
static int output_spec();
static int empty();

static void switch_var_type(int type);

/* initilize LL parse */
extern int init_ll_parse(char *filename){
    /* initilize message buffer */
    memset(errmes, 0, MAXSTRSIZE);

    /* initilize scan */
    if (init_scan(filename) < 0) {
        sprintf(errmes, "File %s can not open", filename);
        return error(errmes);
    }

    init_syntree();
    init_symtab();

    token = scan();
    bflag = 0;
    pflag = 0;
    loop_labels = NULL;
    curvar = NULL;
    trueflag = SFALSE;
    falseflag = SFALSE;

    return 0;
}

/* end LL parse */
extern void end_ll_parse(void){
    release_data(); // release syntax tree
    release_symtab(); // release symbol table
    end_scan();
}

/* create error message */
void create_errmes(char *mes){
    sprintf(errmes, "Line %d: %s\n next token: %d", get_linenum(), mes, token);
}

/*
 * start parse program
 * program ::= "program" "NAME" ";" block "."
 */
extern int parse_program(void){
    char *label;
    init_node();

    if(token != TPROGRAM){
        create_errmes("Keyword 'program' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token != TNAME) {
        create_errmes("Program name is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token != TSEMI){
        create_errmes("';' is not found");
        return error(errmes);
    }
    register_syntree(token);

    START(name_label(MPROGRAM, string_attr, NONE), NONE);
    LAD(gr0, "0", NONE);
    label = create_label();
    CALL(label, NONE);
    CALL("FLUSH", NONE);
    SVC("0", NONE);

    token = scan();
    if(block(label) == ERROR){
        return ERROR;
    }

    if(token != TDOT){
        create_errmes("'.' is not found at the end of program");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();

    end_list_node();
    return 0;
}

/* block ::= {var_declare | sub_program} compound */
static int block(char *label){
    init_node();

    while(token == TVAR || token == TPROCEDURE){
        switch_scope(GLOBAL);
        switch (token) {
            case TVAR:
                if(var_declare() == ERROR){
                    return ERROR;
                }
                break;
            case TPROCEDURE:
                if(sub_program() == ERROR){
                    return ERROR;
                }
                break;
            default: /* never pass */
                return ERROR;
        }
    }

    switch_scope(GLOBAL);
    set_label(label);
    if(compound() == ERROR){
        return ERROR;
    }

    RET();

    end_list_node();
    end_scope(GLOBAL);
    return 0;
}

/* var_declare ::= "var" arrange_var ":" type ";" {arrange_var ":" type ";"} */
static int var_declare(){
    init_node();
    switch_varorpara(VARIABLE);

    if(token != TVAR){
        create_errmes("Keyword 'var' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(arrange_var() == ERROR){
        return ERROR;
    }

    if(token != TCOLON){
        create_errmes("':' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(type() == ERROR){
        return ERROR;
    }

    if(token != TSEMI){
        create_errmes("';' is not found.");
        return error(errmes);
    }
    register_syntree(token);

    while((token = scan()) == TNAME){
        if(arrange_var() == ERROR){
            return ERROR;
        }

        if(token != TCOLON){
            create_errmes("':' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
        if(type() == ERROR){
            return ERROR;
        }

        if(token != TSEMI){
            create_errmes("';' is not found");
            return error(errmes);
        }
        register_syntree(token);
    }

    end_list_node();
    return 0;
}

/* arrange_var ::= var_name {"," var_name} */
static int arrange_var(){
    init_node();

    if(token != TNAME){
        create_errmes("Variable name is not found");
        return error(errmes);
    }
    if(register_symtab(string_attr) == ERROR){
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    while(token == TCOMMA){
        register_syntree(token);

        token = scan();
        if(token != TNAME){
            create_errmes("Variable name is not found");
            return error(errmes);
        }
        if(register_symtab(string_attr) == ERROR){
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
    }

    end_list_node();
    return 0;
}

/* type ::= standard_type | array_type */
static int type(){
    int type;
    int arraysize = 0;

    switch (token) {
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if((type = standard_type()) == ERROR){
                return ERROR;
            }
            break;
        case TARRAY:
            if((type = array_type()) == ERROR){
                return ERROR;
            }
            arraysize = num_attr;
            break;
        default:
            create_errmes("Type keyword('integer', 'boolean', 'char', 'array') is not found");
            return error(errmes);
            break;
    }

    register_type(type, arraysize);
    return 0;
}

/* standard_type ::= "integer" | "boolean" | "char" */
static int standard_type(){
    int type = 0;

    switch (token) {
        case TINTEGER:
            type = TPINT;
            register_syntree(token);
            break;
        case TBOOLEAN:
            type = TPBOOL;
            register_syntree(token);
            break;
        case TCHAR:
            type = TPCHAR;
            register_syntree(token);
            break;
        default:
            create_errmes("Type keyword('integer', 'boolean', 'char') is not found");
            return error(errmes);
            break;
    }
    token = scan();

    //end_list_node();
    return type;
}

/* array_type ::= "array" "[" "NUMBER" "]" "of" standard_type */
static int array_type(){
    int type;
    init_node();

    if(pflag != 0){
        create_errmes("can not use an array type as a formal parameter type");
        return error(errmes);
    }

    if(token != TARRAY){
        create_errmes("Keyword 'array' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token != TLSQPAREN){
        create_errmes("'[' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token != TNUMBER){
        create_errmes("invalid error");
        return error(errmes);
    }
    if(num_attr == 0){
        create_errmes("array size is 1 or more");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token != TRSQPAREN){
        create_errmes("']' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token != TOF){
        create_errmes("Keyword 'of' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if((type = standard_type()) == ERROR){
        return ERROR;
    }

    end_list_node();
    return type;
}

/* sub_program ::= "procedure" procedure_name [parameter] ";" [var_declare] compound ";" */
static int sub_program(){
    char *label;
    UNDEFINE_LIST *paramlist = NULL;
    sbool flag = SFALSE;
    init_node();

    if(token != TPROCEDURE){
        create_errmes("Keyword 'procedure' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token != TNAME){
        create_errmes("procedure name is not found");
        return error(errmes);
    }
    register_syntree(token);
    if(register_subpro(string_attr) == ERROR){
        return error(errmes);
    }
    label = name_label(MSUBPROGRAM, string_attr, NONE);

    switch_scope(LOCAL);
    token = scan();
    if(token == TLPAREN){
        if(parameter(&paramlist) == ERROR){
            return ERROR;
        }
    }

    if(token != TSEMI){
        create_errmes("';' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token == TVAR){
        if(var_declare() == ERROR){
            return ERROR;
        }
    }

    set_label(label);
    if(paramlist != NULL){
        POP(gr2);
        flag = STRUE;
    }
    while(paramlist != NULL){
        UNDEFINE_LIST *tmp = paramlist;
        paramlist = tmp->nextp;

        POP(gr1); // gr1 <-- address
        ST(gr1, name_label(MPARAMETER, tmp->idp->name, tmp->idp->procname), NONE);

        free(tmp);
    }
    if(flag == STRUE){
        PUSH("0", gr2);
    }
    reset_paramlist();

    if(compound() == ERROR){
        return ERROR;
    }

    if(token != TSEMI){
        create_errmes("';' is not found");
        return error(errmes);
    }
    register_syntree(token);

    RET();

    token = scan();

    end_list_node();
    end_scope(LOCAL);
    return 0;
}

/* parameter ::= "(" arrange_var ":" type {";" arrange_var ":" type} ")" */
static int parameter(UNDEFINE_LIST **list){
    init_node();
    switch_varorpara(PARAMETER);
    pflag = 1;

    if(token != TLPAREN){
        create_errmes("'(' is not found");
        return error(errmes);
    }
    register_syntree(token);

    /* may be able to write do-while  */
    token = scan();
    if(arrange_var() == ERROR){
        return ERROR;
    }

    if(token != TCOLON){
        create_errmes("':' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(type() == ERROR){
        return ERROR;
    }

    while(token == TSEMI){
        register_syntree(token);

        token = scan();
        if(arrange_var() == ERROR){
            return ERROR;
        }

        if(token != TCOLON){
            create_errmes("':' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
        if(type() == ERROR){
            return ERROR;
        }
    }
    if(token != TRPAREN){
        create_errmes("')' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();

    pflag = 0;
    *list = register_param();
    end_list_node();
    return 0;
}

/* compound ::= "begin" statement {";" statement} "end" */
static int compound(){
    init_node();

    if(token != TBEGIN){
        create_errmes("Keyword 'begin' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(statement() == ERROR){
        return ERROR;
    }

    while (token == TSEMI) {
        register_syntree(token);

        token = scan();
        if(statement() == ERROR){
            return ERROR;
        }
    }
    if(token != TEND){
        create_errmes("Keyword 'end' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();

    end_list_node();
    return 0;
}

/* statement ::= assign | branch | loop | escase | call_procedure | back | input | output | compound | empty */
static int statement(){
    //init_node();
    switch (token) {
        case TNAME:
            if(assign() == ERROR){
                return ERROR;
            }
            break;
        case TIF:
            if(branch() == ERROR){
                return ERROR;
            }
            break;
        case TWHILE:
            if(loop() == ERROR){
                return ERROR;
            }
            break;
        case TBREAK:
            if(escape() == ERROR){
                return ERROR;
            }
            break;
        case TCALL:
            if(call_procedure() == ERROR){
                return ERROR;
            }
            break;
        case TRETURN:
            if(back() == ERROR){
                return ERROR;
            }
            break;
        case TREAD:
        case TREADLN:
            if(input() == ERROR){
                return ERROR;
            }
            break;
        case TWRITE:
        case TWRITELN:
            if(output() == ERROR){
                return ERROR;
            }
            break;
        case TBEGIN:
            if(compound() == ERROR){
                return ERROR;
            }
            break;
        default:
            if(empty() == ERROR){
                return ERROR;
            }
            break;
    }

    //end_list_node();
    return 0;
}

/* branch ::= "if" expression "then" statement ["else" statement] */
static int branch(){
    int type;
    init_node();

    if(token != TIF){
        create_errmes("Keyword 'if' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if((type = expression()) == ERROR){
        return ERROR;
    }
    if(type != TPBOOL){
        create_errmes("expression is not boolean type");
        return error(errmes);
    }

    // gr1 is expression value
    char *label1 = create_label(); // for true label
    // POP(gr1);          // stack top is expression value
    CPA_rr(gr1, gr0);  // compare gr1(true or flase), gr0(false)
    JZE(label1, NONE); // gr1 == gr0 --> false --> JUMP label1

    if(token != TTHEN){
        create_errmes("Keyword 'then' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(statement() == ERROR){
        return ERROR;
    }

    if(token == TELSE){
        register_syntree(token);

        char *label2 = create_label(); // for false label
        JUMP(label2, NONE); // if expression value is true, JUMP label2
        set_label(label1);  // if false, execute the following statement

        token = scan();
        if(statement() == ERROR){
            return ERROR;
        }

        set_label(label2);
    }else{
        set_label(label1); // if there is not keyword "else" and expression value is false, JUMP here(not execute)
    }

    end_list_node();
    return 0;
}

/* loop ::= "while" expression "do" statement */
static int loop(){
    int type;
    struct LABELS *p;

    init_node();
    bflag++;

    if(token != TWHILE){
        create_errmes("Keyword 'while' is not found");
        return error(errmes);
    }
    register_syntree(token);

    char *label1 = create_label(); // for true label
    char *label2 = create_label(); // for false label
    set_label(label1); // if expression value is true, JUMP label1

    token = scan();
    if((type = expression()) == ERROR){
        return ERROR;
    }
    if(type != TPBOOL){
        create_errmes("expression is not boolean type");
        return error(errmes);
    }

    // gr1 is expression value
    // POP(gr1);          // stack top is expression value
    CPA_rr(gr1, gr0);  // compare gr1(true or flase), gr0(false)
    JZE(label2, NONE); // gr1 == gr0 --> false --> JUMP label2

    if((p = (struct LABELS *)malloc(sizeof(struct LABELS))) == NULL){
        printf("can not malloc in loop\n");
        return ERROR;
    }
    p->label = label2;
    p->next = loop_labels;
    loop_labels = p;

    if(token != TDO){
        create_errmes("Keyword 'do' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(statement() == ERROR){
        return ERROR;
    }

    JUMP(label1, NONE); // after execute the above statement, JUMP label1
    set_label(label2);  // if false, JUMP label2(finish while-loop process)

    loop_labels = loop_labels->next;
    free(p);

    bflag--;
    end_list_node();
    return 0;
}

/* escape ::= "break" */
static int escape(){
    if(token != TBREAK){
        create_errmes("Keyword 'break' is not found");
        return error(errmes);
    }
    /* Keyword 'break' is found, expressionept for loop statement */
    if(bflag == 0){
        create_errmes("'break' is only used loop statement");
        return error(errmes);
    }
    register_syntree(token);

    JUMP(loop_labels->label, NONE);

    token = scan();

    return 0;
}

/* call_procedure ::= "call" procedure_name ["(" arrange_expression ")"] */
static int call_procedure(){
    int count, para;
    init_node();

    if(token != TCALL){
        create_errmes("Keyword 'call' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token != TNAME){
        create_errmes("procedure name is not found");
        return error(errmes);
    }
    register_syntree(token);
    if((count = reference_subpro(string_attr)) == ERROR){
        return error(errmes);
    }
    char *label = name_label(MSUBPROGRAM, string_attr, NONE); // get subprogram name label

    token = scan();
    if(token == TLPAREN){
        register_syntree(token);

        token = scan();
        if((para = arrange_expression()) == ERROR){
            return ERROR;
        }

        if(count != para){
            if(count > para){
                create_errmes("formal parameter is too much");
            }else{
                create_errmes("formal parameter is too less");
            }
            return error(errmes);
        }

        if(token != TRPAREN){
            create_errmes("')' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
    }

    CALL(label, NONE); // JUMP subprogram address

    end_list_node();
    return 0;
}

/* arrange_expression ::= expression {"," expression} */
static int arrange_expression(){
    int type;
    int count = 0;

    init_node();
    /* may be able to write do-while */
    switch_var_type(ARG_VAR);
    if((type = expression()) == ERROR){
        return ERROR;
    }
    count++;
    if(type != param_type(count)){
        char buf[MAXSTRSIZE];
        sprintf(buf, "the %d paramter dont much type", count);
        create_errmes(buf);
        return error(errmes);
    }

    /*
     * expression type
     *   variable: stack variable address
     *   other: allocate working area
     *     and expression value store at this area
     */
    if(curvar != NULL){ // variable
        LAD(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), NONE);
        PUSH("0", gr1);
    }else{ // other
        char *label = create_label();
        set_required(label, NULL);
        LAD(gr2, label, NONE);
        ST(gr1, "0", gr2);
        PUSH("0", gr2);
    }

    while (token == TCOMMA) {
        register_syntree(token);

        token = scan();
        switch_var_type(ARG_VAR);
        if((type = expression()) == ERROR){
            return ERROR;
        }
        count++;
        if(type != param_type(count)){
            char buf[MAXSTRSIZE];
            sprintf(buf, "the %d paramter dont much type", count);
            create_errmes(buf);
            return error(errmes);
        }

        if(curvar != NULL){ // variable
            LAD(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), NONE);
            PUSH("0", gr1);
        }else{ // other
            char *label = create_label();
            set_required(label, NULL);
            LAD(gr2, label, NONE);
            ST(gr1, "0", gr2);
            PUSH("0", gr2);
        }
    }

    end_list_node();
    return count;
}

/* back ::= "return" */
static int back(){
    //init_node();

    if(token != TRETURN){
        create_errmes("Keyword 'return' is not found");
        return error(errmes);
    }
    register_syntree(token);

    RET();

    token = scan();

    //end_list_node();
    return 0;
}

/* assign ::= var ":=" expression */
static int assign(){
    int ltype, rtype;
    init_node();

    switch_var_type(LEFT_VAR);
    if((ltype = var()) == ERROR){ // left_hand is only var
        return ERROR;
    }
    if(ltype == TPARRAY){
        create_errmes("can not assign to array type");
        return error(errmes);
    }

    if(token != TASSIGN){
        create_errmes("':=' is not found");
        return error(errmes);
    }
    register_syntree(token);

    if(curvar->ispara == PARAMETER){
        LD_rm(gr1, name_label(MPARAMETER, curvar->name, curvar->procname), NONE);
        PUSH("0", gr1);
    }

    token = scan();
    switch_var_type(RIGHT_VAR);
    if((rtype = expression()) == ERROR){
        return ERROR;
    }
    if(rtype == TPARRAY){
        create_errmes("can not assign from array type");
        return error(errmes);
    }

    if(ltype != rtype){
        char buf[MAXSTRSIZE];
        sprintf(buf, "dont allow to assign to '%s' from '%s'", type_str[ltype], type_str[rtype]);
        create_errmes(buf);
        return error(errmes);
    }

    /*
     * gr1 <-- right expression value
     * left value
     *     standard variable: direct assign to variable address
     *          no use other register
     *     array variable: assign to variable address + element
     *          gr2 <-- element value
     *     parameter: assign to argument address
     *          gr2 <-- argument address
     */

    // POP(gr1); // right expression value
    if(curvar->ispara == PARAMETER){ // parameter
        POP(gr2); // argument address
        ST(gr1, "0", gr2);
    }else if(curvar->itype->ttype == TPARRAY){ // array variable
        POP(gr2); // element value
        ST(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), gr2);
    }else{ // standard variable
        ST(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), NONE);
    }
    curvar = NULL;

    end_list_node();
    return 0;
}

/* var ::= var_name ["[" expression "]"] */
static int var(){
    int type, etype;
    init_node();

    if(token != TNAME){
        create_errmes("variable name is not found");
        return error(errmes);
    }
    register_syntree(token);
    if((type = reference_name(string_attr)) == ERROR){
        return error(errmes);
    }

    ID *var = search_symtab(string_attr);

    token = scan();
    if(token == TLSQPAREN){
        int vtype = get_array_element_type(string_attr);
        if(type != TPARRAY){
            char buf[MAXSTRSIZE];
            sprintf(buf, "%s is not array type", string_attr);
            create_errmes(buf);
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
        int tvar = type_curvar;
        switch_var_type(RIGHT_VAR);
        if((etype = expression()) == ERROR){
            return ERROR;
        }
        if(etype != TPINT){
            create_errmes("array subscript is not an integer");
            return error(errmes);
        }
        switch_var_type(tvar);

        if(token != TRSQPAREN){
            create_errmes("']' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();

        type = vtype;

        // gr1 <-- expression value

        // POP(gr1); // gr1 <-- expression value
        LAD(gr2, itoa(var->itype->arraysize), NONE); // gr2 <-- arraysize
        CPA_rr(gr2, gr1); // whether arraysize <= gr1
        JMI("EROV", NONE); // gr2 is in array size range < gr1: ERROR
        JZE("EROV", NONE); // gr2 = gr1: ERROR(MAX:arraysize-1)
        CPA_rr(gr1, gr0);  // whether gr1 < 0: ERROR
        JMI("EROV", NONE);
        if(type_curvar == RIGHT_VAR){
            int lscope = (var->ispara == PARAMETER) ? MPARAMETER : MVARIABLE;
            char *label = name_label(lscope, var->name, var->procname);
            LD_rm(gr1, label, gr1); // gr1 <-- (variable + gr1)
            // PUSH("0", gr1);
        }else{
            PUSH("0", gr1); // stack top is element value
        }
    }else{
        if(type_curvar == RIGHT_VAR){
            int lscope = (var->ispara == PARAMETER) ? MPARAMETER : MVARIABLE;
            char *label = name_label(lscope, var->name, var->procname);
            LD_rm(gr1, label, NONE);
            if(var->ispara == PARAMETER){
                LD_rm(gr1, "0", gr1);
            }
            // PUSH("0", gr1);
        }
    }

    if(type_curvar == LEFT_VAR || type_curvar == ARG_VAR){
        curvar = var;
    }

    end_list_node();
    return type;
}

/* expression ::= simple_expression {relatinal_operator simple_expression} */
static int expression(){
    int type, ltype, rtype;
    int opr;

    init_node();

    if((type = simple_expression()) == ERROR){
        return ERROR;
    }
    ltype = type;

    // gr1 is simple expression value

    while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
        if(type_curvar == ARG_VAR && curvar != NULL){
            LD_rm(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), NONE);
            curvar = NULL;
        }
        opr = token;
        if((type = relatinal_operator()) == ERROR){
            return ERROR;
        }

        PUSH("0", gr1); // stack left value

        int tmp = type_curvar;
        switch_var_type(RIGHT_VAR);
        if((rtype = simple_expression()) == ERROR){
            return ERROR;
        }
        switch_var_type(tmp);

        if(ltype == TPARRAY || rtype == TPARRAY){
            create_errmes("dont allow array operand");
            return error(errmes);
        }
        if(ltype != rtype){
            char buf[MAXSTRSIZE];
            sprintf(buf, "can not compare '%s' and '%s'", type_str[ltype], type_str[rtype]);
            create_errmes(buf);
            return error(errmes);
        }

        // POP(gr1); // gr1 <-- right value
        POP(gr2); // gr2 <-- left value
        CPA_rr(gr2, gr1); // gr2 comp ge1
        switch (opr) {
            case TEQUAL:{
                // whether gr2 = gr3 or not
                // gr2 = gr3 --> ZF:1 --> true
                char *equal = create_label();
                char *label = create_label();
                JZE(equal, NONE); // gr2 = gr1 --> JUMP EQUAL label
                LD_rr(gr1, gr0); // gr1 <-- 0(false)
                JUMP(label, NONE); // gr2 <> gr1
                set_label(equal); // gr2 = gr1
                LAD(gr1, OBJTRUE, NONE); // gr1 <-- 1(true)
                set_label(label);
                break;
            }
            case TNOTEQ:{
                // whether gr2 <> gr3 or not
                // gr2 <> gr3 --> ZF:0 --> true
                char *noteq = create_label();
                char *label = create_label();
                JNZ(noteq, NONE); // gr2 <> gr1 --> JUMP NOTEQ label
                LD_rr(gr1, gr0); // gr1 <-- 0(false)
                JUMP(label, NONE); // gr2 = gr1
                set_label(noteq); // gr2 <> gr1
                LAD(gr1, OBJTRUE, NONE); // gr1 <-- 1(true)
                set_label(label);
                break;
            }
            case TLE:{
                // whether gr2 < gr3 or not
                // gr2 < gr3 --> SF:1, ZF:0 --> true
                char *le = create_label();
                char *label = create_label();
                JMI(le, NONE); // gr2 < gr1 --> JUMP LE label
                LD_rr(gr1, gr0); // gr1 <-- 0(false)
                JUMP(label, NONE); // gr2 >= gr1
                set_label(le); // gr2 < gr1
                LAD(gr1, OBJTRUE, NONE); // gr1 <-- 1(true)
                set_label(label);
                break;
            }
            case TLEEQ:{
                // whether gr2 <= gr1 or not
                // gr2 <= gr1 --> SF:1, ZF:1 --> true
                char *leeq = create_label();
                char *label = create_label();
                // JMI(leeq, NONE); // gr2 < gr3 --> JUMP LEEQ label
                // JZE(leeq, NONE); // gr2 = gr3 --> JUMP LEEQ label
                JPL(leeq, NONE); // gr2 > gr1
                LAD(gr1, OBJTRUE, NONE); // gr1 <-- 1(true)
                JUMP(label, NONE); // gr2 > gr1
                set_label(leeq); // gr2 <= gr1
                LD_rr(gr1, gr0); // gr1 <-- 0(false)
                set_label(label);
                break;
            }
            case TGR:{
                // whether gr2 > gr1 or not
                // gr2 > gr1 --> SF:0, ZF:0 --> true
                char *gr = create_label();
                char *label = create_label();
                JPL(gr, NONE); // gr2 > gr1
                LD_rr(gr1, gr0); // gr1 <-- 0(false)
                JUMP(label, NONE); // gr2 <= gr1
                set_label(gr); // gr2 > gr1
                LAD(gr1, OBJTRUE, NONE); // gr1 <-- 1(true)
                set_label(label);
                break;
            }
            case TGREQ:{
                // whether gr2 >= gr1 or not
                // gr2 >= gr1 --> SF:0, ZF:1 --> true
                char *greq = create_label();
                char *label = create_label();
                // JPL(greq, NONE); // gr2 > gr1
                // JZE(greq, NONE); // gr2 = gr1
                JMI(greq, NONE); // gr2 < gr1
                LAD(gr1, OBJTRUE, NONE); // gr1 <-- 1(true)
                JUMP(label, NONE); // gr2 < gr1
                set_label(greq); // gr2 >= gr1
                LD_rr(gr1, gr0); // gr1 <-- 0(false)
                set_label(label);
                break;
            }
            default:
                break;
        }
        // PUSH("0", gr1);
    }
    end_list_node();
    return type;
}

/* simple_expression ::= ["+"|"-"] term {add_operator term} */
static int simple_expression(){
    int type, ltype, rtype;
    int opr;
    sbool mflag = SFALSE;
    sbool flag = SFALSE;

    init_node();

    if(token == TPLUS || token == TMINUS){
        register_syntree(token);
        flag = STRUE;
        if(token == TMINUS){
            mflag = STRUE;
        }
        token = scan();
    }
    if((type = term()) == ERROR){
        return ERROR;
    }
    if(flag == STRUE && type != TPINT){
        char buf[MAXSTRSIZE];
        sprintf(buf, "dont allow '%s' operand after '+'/'-'", type_str[type]);
        create_errmes(buf);
        return error(errmes);
    }
    ltype = type;

    // gr1 is term value

    if(mflag == STRUE){
        // POP(gr2); // gr2 <-- unsighnd integer
        LD_rr(gr2, gr0); // gr1 <-- 0
        SUBA_rr(gr2, gr1); // gr2 <-- 0 - gr1
        JOV("EOVF", NONE);
        LD_rr(gr1, gr2); // gr1 <-- gr2(-gr1)
        // PUSH("0", gr1);
    }

    // gr1 is term value(processed minus value)

    while (token == TPLUS || token == TMINUS || token == TOR) {
        if(type_curvar == ARG_VAR && curvar != NULL){
            LD_rm(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), NONE);
            curvar = NULL;
        }
        opr = token;
        if((type = add_operator()) == ERROR){
            return ERROR;
        }

        PUSH("0", gr1); // stack left value

        int tmp = type_curvar;
        switch_var_type(RIGHT_VAR);
        if((rtype = term()) == ERROR){
            return ERROR;
        }
        switch_var_type(tmp);

        if(ltype == TPARRAY || rtype == TPARRAY){
            create_errmes("dont allow array operand");
            return error(errmes);
        }
        if(type != ltype || type != rtype || ltype != rtype){
            char buf[MAXSTRSIZE];
            sprintf(buf, "can not add '%s' and '%s'", type_str[ltype], type_str[rtype]);
            create_errmes(buf);
            return error(errmes);
        }

        // POP(gr1); // gr1 <-- right term
        POP(gr2); // gr2 <-- left term
        switch (opr) {
            case TPLUS:
                ADDA_rr(gr1, gr2);
                JOV("EOVF", NONE);
                break;
            case TMINUS:
                SUBA_rr(gr2, gr1); // left term(gr2) - right term(gr1)
                JOV("EOVF", NONE);
                LD_rr(gr1, gr2);
                break;
            case TOR:
                OR_rr(gr1, gr2);
                break;
            default: // never pass
                break;
        }
        // PUSH("0", gr1);
    }

    end_list_node();
    return type;
}

/* term ::= factor {multi_operator factor} */
static int term(){
    int type, ltype, rtype;
    int opr;

    init_node();

    if((type = factor()) == ERROR){
        return ERROR;
    }
    ltype = type;

    // gr1 is factor value.
    // if there is not right factor, do nothing.

    while (token == TSTAR || token == TDIV || token == TAND) {
        if(type_curvar == ARG_VAR && curvar != NULL){
            LD_rm(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), NONE);
            curvar = NULL;
        }
        opr = token;
        if((type = multi_operator()) == ERROR){
            return ERROR;
        }

        PUSH("0", gr1); // stack left value

        int tmp = type_curvar;
        switch_var_type(RIGHT_VAR);
        if((rtype = factor()) == ERROR){
            return ERROR;
        }
        switch_var_type(tmp);

        if(ltype == TPARRAY || rtype == TPARRAY){
            create_errmes("dont allow array operand");
            return error(errmes);
        }
        if(type != ltype || type != rtype || ltype != rtype){
            char buf[MAXSTRSIZE];
            sprintf(buf, "can not %s multiple '%s' and '%s'", type_str[type], type_str[ltype], type_str[rtype]);
            create_errmes(buf);
            return error(errmes);
        }

        // POP(gr1); // right factor value
        POP(gr2); // left factor value
        switch (opr) {
            case TSTAR:
                MULA_rr(gr1, gr2);
                JOV("EOVF", NONE);
                break;
            case TDIV:
                DIVA_rr(gr2, gr1);
                JOV("E0DIV", NONE);
                LD_rr(gr1, gr2);
                break;
            case TAND:
                AND_rr(gr1, gr2);
                break;
            default:
                break;
        }
        // PUSH("0", gr1); // stack result value
    }

    end_list_node();
    return type;
}

/* factor ::= var | constant | "(" expression ")" | "not" factor | standard_type "(" expression ")" */
static int factor(){
    int type;

    switch (token) {
        case TNAME:
            if((type = var()) == ERROR){
                return ERROR;
            }
            // gr1 is var value
            // do nothing
            break;
        case TNUMBER:
        case TTRUE:
        case TFALSE:
        case TSTRING:
            if((type = constant()) == ERROR){
                return ERROR;
            }
            // gr1 is constant value
            // do nothing
            break;
        case TLPAREN:
            register_syntree(token);

            token = scan();
            if((type = expression()) == ERROR){
                return ERROR;
            }
            if(token != TRPAREN){
                create_errmes("')' is not found");
                return error(errmes);
            }
            register_syntree(token);

            token = scan();
            // gr1 is expression value
            // do nothing
            break;
        case TNOT:{
            register_syntree(token);

            token = scan();
            if((type = factor()) == ERROR){
                return ERROR;
            }
            if(type != TPBOOL){
                create_errmes("boolean please");
                return error(errmes);
            }
            // gr1 is factor value
            // POP(gr1); // gr1 <-- var value(true(not 0) or false(0))
            CPA_rr(gr0, gr1); // whether gr1 is 0 or not
            char *not = create_label();
            JZE(not, NONE); // if gr1 is 0, JUMP logical not label
            LAD(gr2, "1", NONE); // gr2 <-- 1
            LD_rr(gr1, gr2); // gr1 <-- true(1) (gr1 may be not 1)
            set_label(not);
            XOR_rr(gr1, gr2); // logical not
            // PUSH("0", gr1);
            break;
        }
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if(token == TINTEGER){
                type = TPINT;
            }else if(token == TBOOLEAN){
                type = TPBOOL;
            }else{
                type = TPCHAR;
            }
            register_syntree(token);

            token = scan();
            if(token != TLPAREN){
                create_errmes("'(' is not found");
                return error(errmes);
            }
            register_syntree(token);

            int etype;
            token = scan();
            if((etype = expression()) == ERROR){
                return ERROR;
            }
            if(etype == TPARRAY){
                create_errmes("expression type should be standard type.(integer , char, boolean)");
                return error(errmes);
            }

            if(token != TRPAREN){
                create_errmes("')' is not found");
                return error(errmes);
            }
            register_syntree(token);

            // gr1 is expression value
            // POP(gr1);
            if(etype == TPINT){
                switch (type) {
                    case TPINT:
                        // do nothing
                        break;
                    case TPBOOL:{
                        // if expression value is not 0, gr1 <-- 1
                        CPA_rr(gr1, gr0);
                        char *castitob = create_label();
                        JZE(castitob, NONE);
                        LAD(gr1, OBJTRUE, 0);
                        set_label(castitob);
                        break;
                    }
                    case TPCHAR:{
                        // do nothing or extract the lower 7 bits
                        LAD(gr2, "127", NONE); // gr2 <-- #007F(127)
                        AND_rr(gr1, gr2); // gr1 and #007F --> #00xx
                        break;
                    }
                }
            }else if(etype == TPBOOL){
                switch (type) {
                    case TPINT:{
                        if(trueflag != STRUE && falseflag != STRUE){
                            CPA_rr(gr0, gr1); // whether gr1 is true or false
                            char *castbtoi = create_label();
                            JZE(castbtoi, NONE);
                            LAD(gr1, OBJTRUE, 0); // gr1 <-- 1 (true)
                            set_label(castbtoi);
                        }else{
                            trueflag = SFALSE;
                            falseflag = SFALSE;
                        }
                        break;
                    }
                    case TPBOOL:
                        // do nothing
                        break;
                    case TPCHAR:{
                        if(trueflag != STRUE && falseflag != STRUE){
                            CPA_rr(gr0, gr1); // whether gr1 is true or false
                            char *castbtoc = create_label();
                            JZE(castbtoc, NONE);
                            LAD(gr1, OBJTRUE, 0); // gr1 <-- 1 (true)
                            set_label(castbtoc);
                            break;
                        }
                    }
                }
            }else if(etype == TPCHAR){
                switch (type) {
                    case TPINT:
                        // do nothing
                        break;
                    case TPBOOL:
                        // do nothing
                        break;
                    case TPCHAR:
                        // do nothing
                        break;
                }
            }
            // PUSH("0", gr1);

            token = scan();
            break;
        default:
            create_errmes("factor token is not found");
            return error(errmes);
            break;
    }

    return type;
}

/* constant ::= "NUMBER" | "false" | "true" | "STRING" */
static int constant(){
    int type;

    switch (token) {
        case TNUMBER:
            type = TPINT;
            LAD(gr1, itoa(num_attr), NONE);
            // PUSH("0", gr1);
            break;
        case TFALSE:
            type = TPBOOL;
            falseflag = STRUE;
            LD_rr(gr1, gr0);
            break;
        case TTRUE:
            type = TPBOOL;
            trueflag = STRUE;
            LAD(gr1, OBJTRUE, NONE);
            break;
        case TSTRING:
            if(length != 1){
                char buf[MAXSTRSIZE];
                sprintf(buf, "length of constant string is 1. %s's length is %d", string_attr, length);
                create_errmes(buf);
                return error(errmes);
            }
            type = TPCHAR;
            int c = (int)(string_attr[0]);
            LAD(gr1, itoa(c), NONE);
            break;
        default:
            create_errmes("constant word is not found");
            return error(errmes);
    }
    register_syntree(token);
    token = scan();

    //end_list_node();
    return type;
}

/* multi_operator ::= "*" |"div" | "and" */
static int multi_operator(){
    int type;

    switch (token) {
        case TSTAR:
        case TDIV:
            type = TPINT;
            break;
        case TAND:
            type = TPBOOL;
            break;
        default:
            create_errmes("multi operator is not found");
            return error(errmes);
            break;
    }
    register_syntree(token);
    token = scan();

    return type;
}

/* add_operator ::= "+" | "-" | "or" */
static int add_operator(){
    int type;

    switch (token) {
        case TPLUS:
        case TMINUS:
            type = TPINT;
            break;
        case TOR:
            type = TPBOOL;
            break;
        default:
            create_errmes("add operator is not found");
            return error(errmes);
            break;
    }
    register_syntree(token);
    token = scan();

    return type;
}

/* relatinal_operator ::= "=" | "<>" | "<" | "<=" | ">" | ">=" */
static int relatinal_operator(){
    switch (token) {
        case TEQUAL:
        case TNOTEQ:
        case TLE:
        case TLEEQ:
        case TGR:
        case TGREQ:
            register_syntree(token);
            break;
        default:
            create_errmes("relatinal operator is not found");
            return error(errmes);
            break;
    }
    token = scan();

    return TPBOOL;
}

/* input ::= ("read" | "readln") ["(" var {"," var} ")"] */
static int input(){
    int type;
    sbool readlnflag = SFALSE;
    init_node();

    if(token != TREAD && token != TREADLN){
        create_errmes("Keyword 'read'/'readln' is not found");
        return error(errmes);
    }
    register_syntree(token);

    if(token == TREADLN){
        readlnflag = STRUE;
    }

    token = scan();
    if(token == TLPAREN){
        register_syntree(token);

        token = scan();
        switch_var_type(LEFT_VAR);
        if((type = var()) == ERROR){
            return ERROR;
        }
        if(!(type == TPINT || type == TPCHAR)){
            create_errmes("invalid type");
            return error(errmes);
        }

        if(curvar->ispara == VARIABLE){
            LAD(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), NONE);
        }else{ // parameter
            LD_rm(gr1, name_label(MPARAMETER, curvar->name, curvar->procname), NONE);
        }
        switch (type) {
            case TPINT:
                CALL("READINT", NONE);
                break;
            case TPCHAR:
                CALL("READCHAR", NONE);
                break;
            default: // never pass
                break;
        }
        curvar = NULL;

        while(token == TCOMMA){
            register_syntree(token);

            token = scan();
            switch_var_type(LEFT_VAR);
            if((type = var()) == ERROR){
                return ERROR;
            }
            if(!(type == TPINT || type == TPCHAR)){
                create_errmes("invalid type");
                return error(errmes);
            }

            if(curvar->ispara == VARIABLE){
                LAD(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), NONE);
            }else{ // parameter
                LD_rm(gr1, name_label(MPARAMETER, curvar->name, curvar->procname), NONE);
            }

            switch (type) {
                case TPINT:
                    CALL("READINT", NONE);
                    break;
                case TPCHAR:
                    CALL("READCHAR", NONE);
                    break;
                default: // never pass
                    break;
            }
            curvar = NULL;
        }
        if(token != TRPAREN){
            create_errmes("')' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
    }

    if(readlnflag == STRUE){
        CALL("READLINE", NONE);
    }

    end_list_node();
    return 0;
}

/* output ::= ("write" | "writeln") ["(" output_spec {"," output_spec} ")"] */
static int output(){
    sbool writelnflag = SFALSE;
    int optputtype;
    init_node();

    if(token != TWRITE && token != TWRITELN){
        create_errmes("Keyword 'write'/'writeln' is not found");
        return error(errmes);
    }
    register_syntree(token);
    if(token == TWRITELN){
        writelnflag = STRUE;
    }

    token = scan();
    if(token == TLPAREN){
        register_syntree(token);

        token = scan();
        if((optputtype = output_spec()) == ERROR){
            return ERROR;
        }
        switch (optputtype) {
            case TPINT:
                CALL("WRITEINT", NONE);
                break;
            case TPCHAR:
                CALL("WRITECHAR", NONE);
                break;
            case TPBOOL:
                CALL("WRITEBOOL", NONE);
                break;
            case TPSTRING:
                CALL("WRITESTR", NONE);
                break;
            default:
                break;
        }

        while(token == TCOMMA){
            register_syntree(token);

            token = scan();
            if((optputtype = output_spec()) == ERROR){
                return ERROR;
            }
            switch (optputtype) {
                case TPINT:
                    CALL("WRITEINT", NONE);
                    break;
                case TPCHAR:
                    CALL("WRITECHAR", NONE);
                    break;
                case TPBOOL:
                    CALL("WRITEBOOL", NONE);
                    break;
                case TPSTRING:
                    CALL("WRITESTR", NONE);
                    break;
                default:
                    break;
            }
        }

        if(token != TRPAREN){
            create_errmes("')' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
    }

    if(writelnflag == STRUE){
        CALL("WRITELINE", NONE);
    }

    end_list_node();
    return 0;
}

/* output_spec ::= expression [":" "NUMBER"] | "STRING" */
static int output_spec(){
    int type;
    init_node();

    if(token == TPLUS || token == TMINUS || token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || (token == TSTRING && length == 1) || token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR){
        switch_var_type(RIGHT_VAR);
        if((type = expression()) == ERROR){ // var <-- LEFT_VAR
            return ERROR;
        }
        if(type == TPARRAY){
            create_errmes("invalid type");
            return error(errmes);
        }

        // gr1 <-- expression value

        // if(curvar != NULL){
        //     if(curvar->itype->ttype == TPARRAY){ // variable type is array
        //         POP(gr1); // gr1 <-- element value
        //         LD_rm(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), gr1);
        //     }else{ // variable type is standard
        //         LD_rm(gr1, name_label(MVARIABLE, curvar->name, curvar->procname), NONE);
        //     }
        // }else{
        //     POP(gr1);
        // }

        if(token == TCOLON){
            register_syntree(token);

            token = scan();
            if(token != TNUMBER){
                create_errmes("invalid syntax: missing unsighnd number");
                return error(errmes);
            }
            register_syntree(token);

            LAD(gr2, itoa(num_attr), NONE);

            token = scan();
        }else{
            LD_rr(gr2, gr0);
        }
    }else{
        if(token != TSTRING){
            create_errmes("invalid syntax: missing string");
            return error(errmes);
        }
        if(!(length == 0 || length >= 2)){
            create_errmes("string length 0 or 2 or more");
            return error(errmes);
        }
        SYNTAX_TREE *node = register_syntree(token);
        type = TPSTRING;

        char *label = create_label();
        set_required(label, node->data.str_pointer);
        LAD(gr1, label, NONE); // gr1 <-- character string
        LD_rr(gr2, gr0); // gr2 <-- 0 : minimum

        token = scan();
    }

    end_list_node();
    return type;
}

/* empty ::= Epsilon */
static int empty(){
    return 0;
}

void switch_var_type(int type){
    type_curvar = type;
}
