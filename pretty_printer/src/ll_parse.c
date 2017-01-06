/* ll_parse.c */

#include "ll_parse.h"

static int token;
static char errmes[MAXSTRSIZE];
static int bflag; // break flag (more 1: in loop, 0: not in loop)
static int pflag; // parameter flag (1: paramter, 0: not parameter)

static char *type_str[] = {"", "integer", "char", "boolean"};

static struct LABELS *loop_labels;

static int block(char *label);
static int var_declare();
static int arrange_var();
// static int var_name(); // it might be omitted
static int type();
static int standard_type();
static int array_type();
static int sub_program();
// static int procedure_name(); // it might be omitted
static int parameter();
static int compound(char *label);
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

    set_label(name_label(MPROGRAM, string_attr, NONE));

    token = scan();
    if(token != TSEMI){
        create_errmes("';' is not found");
        return error(errmes);
    }
    register_syntree(token);

    START(NONE);
    LAD(gr0, "0", NONE);
    label = create_label();
    CALL(label, NONE);
    CALL("FLASH", NONE);
    SVC("0", NONE);

    token = scan();
    if(block() == ERROR){
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
    if(compound(label) == ERROR){
        return ERROR;
    }

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
        if(parameter() == ERROR){
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

    if(compound(label) == ERROR){
        return ERROR;
    }

    if(token != TSEMI){
        create_errmes("';' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();

    end_list_node();
    end_scope(LOCAL);
    return 0;
}

/* parameter ::= "(" arrange_var ":" type {";" arrange_var ":" type} ")" */
static int parameter(){
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
    register_param();
    end_list_node();
    return 0;
}

/* compound ::= "begin" statement {";" statement} "end" */
static int compound(char *label){
    init_node();

    if(token != TBEGIN){
        create_errmes("Keyword 'begin' is not found");
        return error(errmes);
    }
    register_syntree(token);

    set_label(label);

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

    char *label1 = create_label(); // for true label
    POP(gr1);          // stack top is expression value
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
    set_label(label1); // if expression value is true, JUMP label1

    token = scan();
    if((type = expression()) == ERROR){
        return ERROR;
    }
    if(type != TPBOOL){
        create_errmes("expression is not boolean type");
        return error(errmes);
    }

    char *label2 = create_label(); // for false label
    POP(gr1);          // stack top is expression value
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

    JUMP(label, NONE); // JUMP subprogram address

    end_list_node();
    return 0;
}

/* arrange_expression ::= expression {"," expression} */
static int arrange_expression(){
    int type;
    int count = 0;
    init_node();
    /* may be able to write do-while */
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

    while (token == TCOMMA) {
        register_syntree(token);

        token = scan();
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

    if((ltype = var()) == ERROR){ // left_hand is only var
        return ERROR;
    }
    if(ltype == TPARRAY){
        create_errmes("can not assign to array type");
        return error(errmes);
    }

    char *label = name_label(string_attr);

    if(token != TASSIGN){
        create_errmes("':=' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
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

    POP(gr2);

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
        if((etype = expression()) == ERROR){
            return ERROR;
        }
        if(etype != TPINT){
            create_errmes("array subscript is not an integer");
            return error(errmes);
        }

        if(token != TRSQPAREN){
            create_errmes("']' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();

        type = vtype;
    }

    end_list_node();
    return type;
}

/* expression ::= simple_expression {relatinal_operator simple_expression} */
static int expression(){
    int type, ltype, rtype;

    init_node();

    if((type = simple_expression()) == ERROR){
        return ERROR;
    }
    ltype = type;

    while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
        if((type = relatinal_operator()) == ERROR){
            return ERROR;
        }
        if((rtype = simple_expression()) == ERROR){
            return ERROR;
        }
        if(ltype != rtype){
            char buf[MAXSTRSIZE];
            sprintf(buf, "can not compare '%s' and '%s'", type_str[ltype], type_str[rtype]);
            create_errmes(buf);
            return error(errmes);
        }
        if(ltype == TPARRAY || rtype == TPARRAY){
            create_errmes("dont allow array operand");
            return error(errmes);
        }
    }
    end_list_node();
    return type;
}

/* simple_expression ::= ["+"|"-"] term {add_operator term} */
static int simple_expression(){
    int type, ltype, rtype;
    sbool flag = SFALSE;

    init_node();

    if(token == TPLUS || token == TMINUS){
        register_syntree(token);
        flag = STRUE;
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

    while (token == TPLUS || token == TMINUS || token == TOR) {
        if((type = add_operator()) == ERROR){
            return ERROR;
        }
        if((rtype = term()) == ERROR){
            return ERROR;
        }
        if(type != ltype || type != rtype || ltype != rtype){
            char buf[MAXSTRSIZE];
            sprintf(buf, "can not add '%s' and '%s'", type_str[ltype], type_str[rtype]);
            create_errmes(buf);
            return error(errmes);
        }
        if(ltype == TPARRAY || rtype == TPARRAY){
            create_errmes("dont allow array operand");
            return error(errmes);
        }
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

    while (token == TSTAR || token == TDIV || token == TAND) {
        opr = token;
        if((type = multi_operator()) == ERROR){
            return ERROR;
        }
        if((rtype = factor()) == ERROR){
            return ERROR;
        }
        if(type != ltype || type != rtype || ltype != rtype){
            char buf[MAXSTRSIZE];
            sprintf(buf, "can not %s multiple '%s' and '%s'", type_str[type], type_str[ltype], type_str[rtype]);
            create_errmes(buf);
            return error(errmes);
        }
        if(ltype == TPARRAY || rtype == TPARRAY){
            create_errmes("dont allow array operand");
            return error(errmes);
        }

        POP(gr2);
        POP(gr1);
        switch (opr) {
            case TSTAR:
                MULA_rr(gr1, gr2);
                break;
            case TDIV:
                DIVA_rr(gr1, gr2);
                break;
            case TAND:
                AND_rr(gr1, gr2);
                break;
            default:
                break;
        }
        PUSH("0", gr1);
    }

    end_list_node();
    return type;
}

/* factor ::= var | constant | "(" expression ")" | "not" factor | standard_type "(" expression ")" */
static int factor(){
    int type;

    switch (token) {
        case TNAME:
            if((type =var()) == ERROR){
                return ERROR;
            }
            break;
        case TNUMBER:
        case TTRUE:
        case TFALSE:
        case TSTRING:
            if((type = constant()) == ERROR){
                return ERROR;
            }
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
            break;
        case TNOT:
            register_syntree(token);

            token = scan();
            if((type = factor()) == ERROR){
                return ERROR;
            }
            if(type != TPBOOL){
                create_errmes("boolean please");
                return error(errmes);
            }
            break;
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
                create_errmes("");
                return error(errmes);
            }

            if(token != TRPAREN){
                create_errmes("')' is not found");
                return error(errmes);
            }
            register_syntree(token);

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
            break;
        case TFALSE:
        case TTRUE:
            type = TPBOOL;
            break;
        case TSTRING:
            if(length != 1){
                char buf[MAXSTRSIZE];
                sprintf(buf, "length of constant string is 1. %s's length is %d", string_attr, length);
                create_errmes(buf);
                return error(errmes);
            }
            type = TPCHAR;
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
    init_node();

    if(token != TREAD && token != TREADLN){
        create_errmes("Keyword 'read'/'readln' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token == TLPAREN){
        register_syntree(token);

        token = scan();
        if((type = var()) == ERROR){
            return ERROR;
        }
        if(!(type == TPINT || type == TPCHAR)){
            create_errmes("");
            return error(errmes);
        }

        while(token == TCOMMA){
            register_syntree(token);

            token = scan();
            if((type = var()) == ERROR){
                return ERROR;
            }
            if(!(type == TPINT || type == TPCHAR)){
                create_errmes("");
                return error(errmes);
            }
        }
        if(token != TRPAREN){
            create_errmes("')' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
    }

    end_list_node();
    return 0;
}

/* output ::= ("write" | "writeln") ["(" output_spec {"," output_spec} ")"] */
static int output(){
    init_node();

    if(token != TWRITE && token != TWRITELN){
        create_errmes("Keyword 'write'/'writeln' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token == TLPAREN){
        register_syntree(token);

        token = scan();
        if(output_spec() == ERROR){
            return ERROR;
        }

        while(token == TCOMMA){
            register_syntree(token);

            token = scan();
            if(output_spec() == ERROR){
                return ERROR;
            }
        }

        if(token != TRPAREN){
            create_errmes("')' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
    }

    end_list_node();
    return 0;
}

/* output_spec ::= expression [":" "NUMBER"] | "STRING" */
static int output_spec(){
    int type;
    init_node();

    if(token == TPLUS || token == TMINUS || token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || (token == TSTRING && length == 1) || token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR){
        if((type = expression()) == ERROR){
            return ERROR;
        }
        if(type == TPARRAY){
            create_errmes("");
            return error(errmes);
        }

        if(token == TCOLON){
            register_syntree(token);

            token = scan();
            if(token != TNUMBER){
                create_errmes("invalid syntax: missing unsighnd number");
                return error(errmes);
            }
            register_syntree(token);

            token = scan();
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
        register_syntree(token);

        token = scan();
    }

    end_list_node();
    return 0;
}

/* empty ::= Epsilon */
static int empty(){
    return 0;
}
