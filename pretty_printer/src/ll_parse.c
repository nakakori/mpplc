/* ll_parse.c */

#include "ll_parse.h"

static int token;
static char errmes[MAXSTRSIZE];
static int bflag; // break flag (more 1: in loop, 0: not in loop)

static void create_errmes(char *mes);

static int block();
static int var_declare();
static int arrange_var();
// static int var_name(); // it might be omitted
static int type();
static int standard_type();
static int array_type();
static int sub_program();
// static int procedure_name(); // it might be omitted
static int parameter();
static int compound();
static int statement();
static int branch();
static int loop();
static int escape(); // it might be omitted
static int call_procedure();
static int arrange_exp();
static int back(); // it might be omitted
static int assign();
// static int left_hand();
static int var();
static int exp();
static int simple_exp();
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

    token = scan();
    bflag = 0;

    return 0;
}

/* end LL parse */
extern void end_ll_parse(void){
    release_syntree();
    end_scan();
}

/* create error message */
static void create_errmes(char *mes){
    sprintf(errmes, "Line %d: %s\n next token: %d", get_linenum(), mes, token);
}

/* *********************************************
 * start parse program
 * program ::= "program" "NAME" ";" block "."
 * ********************************************* */
extern int parse_program(void){
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
static int block(){
    init_node();

    while(token == TVAR || token == TPROCEDURE){
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

    if(compound() == ERROR){
        return ERROR;
    }

    end_list_node();
    return 0;
}

/* var_declare ::= "var" arrange_var ":" type ";" {arrange_var ":" type ";"} */
static int var_declare(){
    init_node();

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
    register_syntree(token);

    token = scan();
    while(token == TCOMMA){
        register_syntree(token);

        token = scan();
        if(token != TNAME){
            create_errmes("Variable name is not found");
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
    //init_node();

    switch (token) {
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            if(standard_type() == ERROR){
                return ERROR;
            }
            break;
        case TARRAY:
            if(array_type() == ERROR){
                return ERROR;
            }
            break;
        default:
            create_errmes("Type keyword('integer', 'boolean', 'char', 'array') is not found");
            return error(errmes);
            break;
    }

    //end_list_node();
    return 0;
}

/* standard_type ::= "integer" | "boolean" | "char" */
static int standard_type(){
    //init_node();

    switch (token) {
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            register_syntree(token);
            break;
        default:
            create_errmes("Type keyword('integer', 'boolean', 'char') is not found");
            return error(errmes);
            break;
    }
    token = scan();

    //end_list_node();
    return 0;
}

/* array_type ::= "array" "[" "NUMBER" "]" "of" standard_type */
static int array_type(){
    init_node();

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
    if(standard_type() == ERROR){
        return ERROR;
    }

    end_list_node();
    return 0;
}

/* sub_program ::= "procedure" procedure_name [parameter] ";" [var_declare] compound ";" */
static int sub_program(){
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

    if(compound() == ERROR){
        return ERROR;
    }

    if(token != TSEMI){
        create_errmes("';' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();

    end_list_node();
    return 0;
}

/* parameter ::= "(" arrange_var ":" type {";" arrange_var ":" type} ")" */
static int parameter(){
    init_node();

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

/* branch ::= "if" exp "then" statement ["else" statement] */
static int branch(){
    init_node();

    if(token != TIF){
        create_errmes("Keyword 'if' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(exp() == ERROR){
        return ERROR;
    }
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

        token = scan();
        if(statement() == ERROR){
            return ERROR;
        }
    }

    end_list_node();
    return 0;
}

/* loop ::= "while" exp "do" statement */
static int loop(){
    init_node();
    bflag++;

    if(token != TWHILE){
        create_errmes("Keyword 'while' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(exp() == ERROR){
        return ERROR;
    }

    if(token != TDO){
        create_errmes("Keyword 'do' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(statement() == ERROR){
        return ERROR;
    }

    bflag--;
    end_list_node();
    return 0;
}

/* escape ::= "break" */
static int escape(){
    //init_node();

    if(token != TBREAK){
        create_errmes("Keyword 'break' is not found");
        return error(errmes);
    }
    /* Keyword 'break' is found, expept for loop statement */
    if(bflag == 0){
        create_errmes("'break' is only used loop statement");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();

    //end_list_node();
    return 0;
}

/* call_procedure ::= "call" procedure_name ["(" arrange_exp ")"] */
static int call_procedure(){
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

    token = scan();
    if(token == TLPAREN){
        register_syntree(token);

        token = scan();
        if(arrange_exp() == ERROR){
            return ERROR;
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

/* arrange_exp ::= exp {"," exp} */
static int arrange_exp(){
    init_node();
    /* may be able to write do-while */
    if(exp() == ERROR){
        return ERROR;
    }

    while (token == TCOMMA) {
        register_syntree(token);

        token = scan();
        if(exp() == ERROR){
            return ERROR;
        }
    }

    end_list_node();
    return 0;
}

/* back ::= "return" */
static int back(){
    //init_node();

    if(token != TRETURN){
        create_errmes("Keyword 'return' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();

    //end_list_node();
    return 0;
}

/* assign ::= left_hand ":=" exp */
static int assign(){
    init_node();

    if(var() == ERROR){ // left_hand is only var
        return ERROR;
    }

    if(token != TASSIGN){
        create_errmes("':=' is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(exp() == ERROR){
        return ERROR;
    }

    end_list_node();
    return 0;
}

/* var ::= var_name ["[" exp "]"] */
static int var(){
    init_node();

    if(token != TNAME){
        create_errmes("variable name is not found");
        return error(errmes);
    }
    register_syntree(token);

    token = scan();
    if(token == TLSQPAREN){
        register_syntree(token);

        token = scan();
        if(exp() == ERROR){
            return ERROR;
        }

        if(token == TRSQPAREN){
            create_errmes("']' is not found");
            return error(errmes);
        }
        register_syntree(token);

        token = scan();
    }

    end_list_node();
    return 0;
}

/* exp ::= simple_exp {relatinal_operator simple_exp} */
static int exp(){
    init_node();

    if(simple_exp() == ERROR){
        return ERROR;
    }
    while (token == TEQUAL || token == TNOTEQ || token == TLE || token == TLEEQ || token == TGR || token == TGREQ) {
        if(relatinal_operator() == ERROR){
            return ERROR;
        }
        if(simple_exp() == ERROR){
            return ERROR;
        }
    }
    end_list_node();
    return 0;
}

/* simple_exp ::= ["+"|"-"] term {add_operator term} */
static int simple_exp(){
    init_node();

    if(token == TPLUS || token == TMINUS){
        register_syntree(token);

        token = scan();
    }
    if(term() == ERROR){
        return ERROR;
    }
    while (token == TPLUS || token == TMINUS || token == TOR) {
        if(add_operator() == ERROR){
            return ERROR;
        }
        if(term() == ERROR){
            return ERROR;
        }
    }

    end_list_node();
    return 0;
}

/* term ::= factor {multi_operator factor} */
static int term(){
    init_node();

    if(factor() == ERROR){
        return ERROR;
    }
    while (token == TSTAR || token == TDIV || token == TAND) {
        if(multi_operator() == ERROR){
            return ERROR;
        }
        if(factor() == ERROR){
            return ERROR;
        }
    }

    end_list_node();
    return 0;
}

/* factor ::= var | constant | "(" exp ")" | "not" factor | standard_type "(" exp ")" */
static int factor(){
    //init_node();

    switch (token) {
        case TNAME:
            if(var() == ERROR){
                return ERROR;
            }
            break;
        case TNUMBER:
        case TTRUE:
        case TFALSE:
        case TSTRING:
            if(constant() == ERROR){
                return ERROR;
            }
            break;
        case TLPAREN:
            register_syntree(token);

            token = scan();
            if(exp() == ERROR){
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
            if(factor() == ERROR){
                return ERROR;
            }
            break;
        case TINTEGER:
        case TBOOLEAN:
        case TCHAR:
            register_syntree(token);

            token = scan();
            if(token != TLPAREN){
                create_errmes("'(' is not found");
                return error(errmes);
            }
            register_syntree(token);

            token = scan();
            if(exp() == ERROR){
                return ERROR;
            }

            if(token != TRPAREN){
                create_errmes("')' is not found");
                return error(errmes);
            }
            register_syntree(token);

            token = scan();
        default:
            create_errmes("factor token is not found");
            return error(errmes);
            break;
    }

    //end_list_node();
    return 0;
}

/* constant ::= "NUMBER" | "false" | "true" | "STRING" */
static int constant(){
    //init_node();

    switch (token) {
        case TNUMBER:
        case TFALSE:
        case TTRUE:
        case TSTRING:
            register_syntree(token);
            break;
        default:
            create_errmes("constant word is not found");
            return error(errmes);
            break;
    }
    token = scan();

    //end_list_node();
    return 0;
}

/* multi_operator ::= "*" |"div" | "and" */
static int multi_operator(){ // it might be omitted
    //init_node();
    switch (token) {
        case TSTAR:
        case TDIV:
        case TAND:
            register_syntree(token);
            break;
        default:
            create_errmes("multi operator is not found");
            return error(errmes);
            break;
    }
    token = scan();

    //end_list_node();
    return 0;
}

/* add_operator ::= "+" | "-" | "or" */
static int add_operator(){ // it might be omitted
    //init_node();

    switch (token) {
        case TPLUS:
        case TMINUS:
        case TOR:
            register_syntree(token);
            break;
        default:
            create_errmes("add operator is not found");
            return error(errmes);
            break;
    }
    token = scan();

    //end_list_node();
    return 0;
}

/* relatinal_operator ::= "=" | "<>" | "<" | "<=" | ">" | ">=" */
static int relatinal_operator(){ // it might be omitted
    //init_node();

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

    //end_list_node();
    return 0;
}

/* input ::= ("read" | "readln") ["(" var {"," var} ")"] */
static int input(){
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
        if(var() == ERROR){
            return ERROR;
        }

        while(token == TCOMMA){
            register_syntree(token);

            token = scan();
            if(var() == ERROR){
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

/* output ::= ("write" | "writeln") ["(" output_spec {"," output_spec} ")"] */
static int output(){
    init_node();

    if(token != TWHILE && token != TWRITELN){
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

/* output_spec ::= exp [":" "NUMBER"] | "STRING" */
static int output_spec(){
    init_node();

    if(token == TPLUS || token == TMINUS || token == TNAME || token == TNUMBER || token == TFALSE || token == TTRUE || (token == TSTRING && length == 1) || token == TLPAREN || token == TNOT || token == TINTEGER || token == TBOOLEAN || token == TCHAR){
        if(exp() == ERROR){
            return ERROR;
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
