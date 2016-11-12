/* pretty_print.c */

#include "pretty_print.h"

static char *token_str[] = {
	"",
	"NAME", "program", "var", "array", "of",
	"begin", "end", "if", "then", "else",
	"procedure", "return", "call", "while", "do",
	"not", "or", "div", "and", "char",
	"integer", "boolean", "readln", "writeln", "true",
	"false", "NUMBER", "STRING", "+", "-",
	"*", "=", "<>", "<", "<=",
	">", ">=", "(", ")", "[",
	"]", ":=", ".", ",", ":",
	";", "read","write", "break"
};

int main(int argc, char *argv[]) {
    if(argc < 2){
        return error("File name id not given.");
    }

    if(init_ll_parse(argv[1]) < 0){
        return 0;
    }

    parse_program();

    pretty_print(synroot);

    end_ll_parse();

    return 0;
}

static int indent = 0;
static pbool line = PFALSE; // PTRUE: need indent, PFALSE: no indent
static int comp = 0; // compound count
// static pbool loop = PFALSE;
// static pbool endflag = PFALSE;
// static pbool paramflag = PFALSE;
// static pbool nameflag = PFALSE;
static int branchindent = 0;
static pbool ifflag = PFALSE;
static pbool elseflag = PFALSE; // PTRUE: else, PFALSE: none
void pretty_print(SYNTAX_TREE *root){
    SYNTAX_TREE *p;
    p = root;

    while (p != NULL) {
        /* config before print token */

        /*
         * indent conditions
         * token = "if": branchindent <= indent
         * same branch(if~else-if~else): indent is not changed
         * branchindent save indent value
         * token = "begin": indent--
         */
        if(p->data.token == TIF && branchindent == 0){
            branchindent = indent;
        }else if(p->data.token == TBEGIN){
            indent--;
        }
        /*
         * print previous newline conditions
         * token = "end"
         * token = "else"
         */
        if(p->data.token == TEND){
            print_newline();
            comp--;
            indent--;
        }else if(p->data.token == TELSE){
            print_newline();
            indent = branchindent;
        }
        if(line == PTRUE && p->data.token != 0){
            print_indent();
        }
        /* print token */
        print_token(p->data);

        /* config after print token */
        /*
         * newline conditions
         * token = "begin": indent--
         * token = ";": indent
         * token = "do" or "then"
         * token = "else" and next token != "if"
         */
        if(p->data.token == TSEMI){
            print_newline();
        }else if(p->data.token == TBEGIN){
            if(elseflag == PTRUE) indent++;
            else if(ifflag == PTRUE) indent++;
            print_newline();
            comp++;
            indent++;
        }else if(p->data.token == TDO || p->data.token == TTHEN){
            print_newline();
            indent++;
            if(p->data.token == TTHEN){
                ifflag = PTRUE;
            }
        }else if(p->data.token == TELSE){
            if(p->next != NULL && p->next->child != NULL && p->next->child->data.token != TIF){
                print_newline();
                indent++;
                elseflag = PTRUE;
                branchindent = 0;
            }
        }else if(p->data.token == TEND){
        }
        /*
         * space conditions
         * token = keyword
         * token = "NAME":
         *     next token = operator
         *
         * next token = ";" | "," | ":" |
         *              "(" | ")" | "[" | "]" : no space
         * previous newline: no space
         */
        if(p->data.token != 0){
            if(p->data.token != TSEMI && p->data.token != TBEGIN && p->data.token != TDO && p->data.token != TTHEN && elseflag != PTRUE){
                print_space();
            }
        }

        /* config next token */
        if(p->child != NULL){
            if(p->child->data.token != 0 && comp == 0){
                indent++;
            }
            pretty_print(p->child);
            if(p->child->data.token != 0 && comp == 0){
                indent--;
                if(indent<0) indent = 0;
            }
        }
        p = p->next;
    }
}

void print_token(DATA data){
    if(data.token == TNAME){
        printf("%s", data.id_pointer->name);
    }else if(data.token == TSTRING){
        // change token_str[] => data.str_pointer
        printf("'%s'", token_str[data.token]);
    }else if(data.token != 0){
        printf("%s", token_str[data.token]);
    }
}

void print_indent(void){
    int i;
    for(i=0; i<indent; i++){
        printf("\t");
    }
    line = PFALSE;
}

void print_newline(void){
    printf("\n");

    line = PTRUE;
    if(elseflag == PTRUE){
        elseflag = PFALSE;
        indent--;
    }else if(ifflag == PTRUE){
        ifflag = PFALSE;
        indent--;
    }
    // nameflag = PFALSE;
}

void print_space(void){
    printf(" ");
}
