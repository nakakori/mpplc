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

    if(parse_program() < 0){
		printf("finish parsing program.\n");
		return 0;
	}
	// parse_program();

    pretty_print(synroot);

    end_ll_parse();

    return 0;
}

static int indent = 0;
static pbool line = PFALSE; // PTRUE: need indent, PFALSE: no indent
static int comp = 0; // compound count
static int branch = 0; // branch count
static pbool ifflag = PFALSE;
static pbool elseflag = PFALSE;
static pbool semiflag = PFALSE;
static pbool procflag = PFALSE;
static pbool paramflag = PFALSE;
static pbool nameflag = PFALSE;
static pbool boolflag = PFALSE;
static pbool numflag = PFALSE;
static pbool strflag = PFALSE;
static pbool parflag = PFALSE;

void pretty_print(SYNTAX_TREE *root){
    SYNTAX_TREE *p;
    p = root;
	int branchindent = 0;
	int loopindent = 0;

    while (p != NULL) {
        /* config before print token */

        /*
         * indent conditions
         * token = "if": branchindent <= indent
         * same branch(if~else-if~else): indent is not changed
         * branchindent save indent value
         * token = "begin": indent--
         */
        if(p->data.token == TIF){
			branchindent = indent;
        }else if(p->data.token == TBEGIN){
            indent--;
        }else if(p->data.token == TWHILE){
			loopindent = indent;
		}
        /*
         * print previous newline conditions
         * token = "end"
         * token = "else"
         */
		if(p->data.token != TEND && semiflag == PTRUE){
			semiflag = PFALSE;
		}
        if(p->data.token == TEND){
            if(semiflag == PFALSE){
				print_newline();
			}
            comp--;
            indent--;
        }else if(p->data.token == TELSE){
            print_newline();
            indent = branchindent;
        }
        if(line == PTRUE && p->data.token != 0){
            print_indent();
        }
		/* space */
		if(nameflag == PTRUE){
			if(p->data.token != 0 && !(p->data.token == TLSQPAREN || p->data.token == TRPAREN || p->data.token == TRSQPAREN || p->data.token == TCOMMA || p->data.token == TSEMI || p->data.token == TCOLON)){
				print_space();
				nameflag = PFALSE;
			}
		}
		if(boolflag == PTRUE){
			if(p->data.token != 0 && !(p->data.token == TSEMI)){
				print_space();
				boolflag = PFALSE;
			}
		}
		if(numflag == PTRUE){
			if(p->data.token != 0 && !(p->data.token == TCOMMA ||p->data.token == TSEMI || p->data.token == TRSQPAREN || p->data.token == TRPAREN)){
				print_space();
				numflag = PFALSE;
			}
		}
		if(strflag == PTRUE){
			if(p->data.token != 0 && !(p->data.token == TRPAREN || p->data.token == TCOMMA || p->data.token == TSEMI)){
				print_space();
				strflag = PFALSE;
			}
		}
		if(parflag == PTRUE){
			if(p->data.token != 0 && !(p->data.token == TRPAREN ||  p->data.token == TCOMMA ||p->data.token == TSEMI)){
				print_space();
				parflag = PFALSE;
			}
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
            if(paramflag == PFALSE){
				print_newline();
			}else{
				print_space();
			}
			semiflag = PTRUE;
        }else if(p->data.token == TBEGIN){
            if(ifflag == PTRUE){
				indent++;
			}
            print_newline();
            comp++;
            indent++;
        }else if(p->data.token == TDO){
            print_newline();
            indent++;
        }else if(p->data.token == TTHEN){
			print_newline();
            indent++;
			if(!(p->next->child != NULL && p->next->child->data.token == TIF)){
				ifflag = PTRUE;
			}
		}else if(p->data.token == TELSE){
            if(!(p->next->child != NULL && p->next->child->data.token == TIF)){
                print_newline();
                indent++;
                elseflag = PTRUE;
				branch--;
            }
        }else if(p->data.token == TPROCEDURE){
			procflag = PTRUE;
		}else if(p->data.token == TLPAREN && procflag == PTRUE){
			paramflag = PTRUE;
			procflag = PFALSE;
		}else if(p->data.token == TRPAREN && paramflag == PTRUE){
			paramflag = PFALSE;
		}
        /*
         * space conditions
         * token = keyword
         * token = "NAME":
         *     nameflag -> PTRUE
		 *     nameflag = PTRUE:
		 *         after print some token, reset.
         * next token = ";" | "," | ":" |
         *              "(" | ")" | "[" | "]" : no space
         * previous newline: no space
         */
		if(p->data.token != 0){
			if(nameflag == PTRUE){
				nameflag = PFALSE;
			}
			if(boolflag == PTRUE){
				boolflag = PFALSE;
			}
			if(numflag == PTRUE){
				numflag = PFALSE;
			}
			if(strflag == PTRUE){
				strflag = PFALSE;
			}
			if(parflag == PTRUE){
				parflag = PFALSE;
			}
		}

		if(p->data.token != 0 && !(p->data.token == TSEMI || p->data.token == TBEGIN || p->data.token == TDO || p->data.token == TTHEN || p->data.token == TEND || p->data.token == TLPAREN || p->data.token == TLSQPAREN || p->data.token == TINTEGER || p->data.token == TCHAR || p->data.token == TBOOLEAN || elseflag == PTRUE)){
			if(p->data.token == TNAME || p->data.token == TBREAK || p->data.token == TRETURN){
				nameflag = PTRUE;
			}else if(p->data.token == TTRUE || p->data.token == TFALSE){
				boolflag = PTRUE;
			}else if(p->data.token == TNUMBER){
				numflag = PTRUE;
			}else if(p->data.token == TSTRING){
				strflag = PTRUE;
			}else if(p->data.token == TRPAREN || p->data.token == TRSQPAREN){
				parflag = PTRUE;
			}else if(p->data.token == TWRITE || p->data.token == TWRITELN || p->data.token == TREAD || p->data.token == TREADLN){
				if(p->next != NULL && p->next->data.token == TLPAREN){
					print_space();
				}
			}else if(p->next != NULL && !(p->next->data.token == TRSQPAREN || p->next->data.token == TCOMMA)){
				print_space();
			}else{
				print_space();
			}
        }

		if(p->data.token != 0 && elseflag == PTRUE){
			elseflag = PFALSE;
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
			if(branchindent > 0){
				indent = branchindent;
				ifflag = PFALSE;
			}
			if(loopindent > 0){
				indent = loopindent;
			}
        }
        p = p->next;
    }
}

void print_token(DATA data){
    if(data.token == TNAME){
        printf("%s", data.id_pointer->name);
    }else if(data.token == TSTRING){
        printf("'%s'", data.str_pointer);
    }else if(data.token == TNUMBER){
		printf("%s", data.str_pointer);
	}else if(data.token != 0){
        printf("%s", token_str[data.token]);
    }
}

void print_indent(void){
    int i;
    for(i=0; i<indent; i++){
        // printf("\t");
		printf("    ");
    }
    line = PFALSE;
}

void print_newline(void){
    printf("\n");

    line = PTRUE;
    if(elseflag == PTRUE){
        elseflag = PFALSE;
    }
	if(ifflag == PTRUE){
        ifflag = PFALSE;
        indent--;
    }
	if(semiflag == PTRUE){
		semiflag = PFALSE;
	}
	if(nameflag == PTRUE){
		nameflag = PFALSE;
	}
	if(boolflag == PTRUE){
		boolflag = PFALSE;
	}
	if(numflag == PTRUE){
		numflag = PFALSE;
	}
	if(strflag == PTRUE){
		strflag = PFALSE;
	}
	if(parflag == PTRUE){
		parflag = PFALSE;
	}
	if(procflag == PTRUE){
		procflag = PFALSE;
	}
}

void print_space(void){
    printf(" ");
}
