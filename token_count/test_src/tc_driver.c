#include "tc_driver.h"

/* string of each token */
char *test_token_list[NUMOFTOKEN+1] = {
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

void test_main(void){
    int buf;

    printf("start test.\n");
    printf("if you input characters for tests, input 0.\n");
    printf("if you use test files, input '1'. \n");
    scanf("%d", &buf);
    if( buf == 1 ){
        scan_file_driver(); /* while test */
    }else{
		scan_self_driver();
	}
}

void scan_file_driver(void){
	char *test_file[7] = {
		"./tests/alphabet_test.mpl",
		"./tests/number_test.mpl",
		"./tests/symbol_test.mpl",
		"./tests/string_test.mpl",
		"./tests/separator_test.mpl",
		"./tests/newline_test.mpl",
		"./tests/comment_test.mpl"
	};

	int i;

	for( i=0; i<7; i++ ){
		int token;
		init_scan(test_file[i]);
		while((token = scan()) >= 0){
			if( token == TNUMBER ){
				printf("%d is ok\n", num_attr);
			}else if( token == TSTRING ){
				printf("%s\n", string_attr);
			}else{
				printf("%s is ok\n", test_token_list[token]);
			}
		}
		if( i == 4 ){
			printf("seprator num:%d\n", get_separator_num());
		}
		if( i == 5 ){
			printf("newline num:%d\n", get_linenum());
		}
		if( i == 6 ){
			printf("comment num:%d\n", get_comment_num());
		}
	}
}

static char buf[MAXSTRSIZE] = {};
static int point;
static int len;
void scan_self_driver(void){
	switch_getc();
	printf("if you quit test program, input 'q'.\n");
	while(strncmp(buf, "q", MAXSTRSIZE) != 0){
		point = 0;
		scanf(" %s", buf);
		len = strlen(buf);
		init_scan("self test");
		printf("'%s' is %s\n", buf, test_token_list[scan()]);
	}
}

int get_buf_string(void){
	if(point == len){
		return EOF;
	}
	return buf[point++];
}
