#include "token-list.h"

/* keyword list */
struct KEY key[KEYWORDSIZE] = {
	{"and", 	TAND	},
	{"array",	TARRAY	},
	{"begin",	TBEGIN	},
	{"boolean",	TBOOLEAN},
	{"break",	TBREAK  },
	{"call",	TCALL	},
	{"char",	TCHAR	},
	{"div",		TDIV	},
	{"do",		TDO	},
	{"else",	TELSE	},
	{"end",		TEND	},
	{"false",	TFALSE	},
	{"if",		TIF	},
	{"integer",	TINTEGER},
	{"not",		TNOT	},
	{"of",		TOF	},
	{"or",		TOR	},
	{"procedure", TPROCEDURE},
	{"program",	TPROGRAM},
	{"read",	TREAD	},
	{"readln",	TREADLN },
	{"return", 	TRETURN },
	{"then",	TTHEN	},
	{"true",	TTRUE	},
	{"var",		TVAR	},
	{"while",	TWHILE	},
	{"write",	TWRITE  },
	{"writeln",	TWRITELN}
};

/* Token counter */
int numtoken[NUMOFTOKEN+1];

/* string of each token */
char *tokenstr[NUMOFTOKEN+1] = {
	"",
	"NAME      ", "program   ", "var       ", "array     ", "of        ",
	"begin     ", "end       ", "if        ", "then      ", "else      ",
	"procedure ", "return    ", "call      ", "while     ", "do        ",
	"not       ", "or        ", "div       ", "and       ", "char      ",
	"integer   ", "boolean   ", "readln    ", "writeln   ", "true      ",
	"false     ", "NUMBER    ", "STRING    ", "+         ", "-         ",
	"*         ", "=         ", "<>        ", "<         ", "<=        ",
	">         ", ">=        ", "(         ", ")         ", "[         ",
	"]         ", ":=        ", ".         ", ",         ", ":         ",
	";         ", "read      ","write      ", "break     "
};

// int tc_main(int nc, char *np[]) {
// 	#ifdef TEST
// 	/* start test */
// 	test_main();
// 	return 0;
// 	#endif
//
// 	int token, i;
//
// 	if (nc < 2) {
// 		printf("File name id not given.\n");
// 		return 0;
// 	}
// 	if (init_scan(np[1]) < 0) {
// 		printf("File %s can not open.\n", np[1]);
// 		return 0;
// 	}
//
// 	/* initialize token counter */
// 	for ( i=0; i < NUMOFTOKEN+1; i++ ){
// 		numtoken[i] = 0;
// 	}
//
// 	/* each token count up */
// 	while ((token = scan()) >= 0) {
// 		numtoken[token]++;
// 		if( token == TNAME ) id_countup(string_attr);
// 	}
// 	end_scan();
//
// 	/* output token counter */
// 	for ( i=1; i < NUMOFTOKEN+1; i++ ){
// 		if(numtoken[i] != 0){
// 			printf("\"%s\"\t%d\n", tokenstr[i], numtoken[i]);
// 			if( i == TNAME ) print_idtab();
// 		}
// 	}
// 	printf("Line count:\t%d\n", get_linenum());
//
// 	release_idtab();
// 	return 0;
// }

int error(char *mes) {
	printf("\n ERROR: %s\n", mes);
	end_scan();
	return ERROR;
}

/* get keyword of the number of n */
struct KEY get_keyword(int n){
	return key[n];
}
