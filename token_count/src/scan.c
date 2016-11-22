#include "scan.h"

/* External valiable */
int num_attr;
char string_attr[MAXSTRSIZE];
int length;

/* Internal variable */
static FILE *fp;
static int cbuf;
static int line_num;
#ifdef TEST
static int separator_num;
static int comment_num;
#endif

/* Use to distinguish between bracket and slash-astarisc */
static int comment_type;
#define COMMENT_BRACKET 1
#define COMMENT_SLASH 2

static enum CHAR_T check_char(int c);
static sbool is_alphabet(int c);
static sbool is_number(int c);
static sbool is_symbol(int c);
static sbool is_string(int c);
static sbool is_separator(int c);
static sbool is_newline(int c);
static sbool is_comment(int c);

static void init_string_attr();

static int get_key_token(void);
static int get_number_token(void);
static int get_symbol_token(void);
static int get_string_token(void);
static void skip_comment(void);

static int file_getc(void);
#ifdef TEST
static int array_getc(void);
#endif
static int (*get_char)(void) = file_getc;

/* initialize scan program */
int init_scan(char *filename){
	#ifdef TEST
	if(strncmp(filename, "self test", strlen(filename)) == 0 ){
		cbuf = get_char();
		return 0;
	}
	#endif

	if((fp = fopen(filename,"r")) == NULL){
		return ERROR;
	}

	if((cbuf = get_char()) == EOF){
		printf("this file is mpty file.\n");
		line_num = 0;
	}else{
		line_num = 1;
	}
	length = 0;

	#ifdef TEST
	int separator_num = 0;
	int comment_num = 0;
	#endif

	return 0;
}

/* scan token and return tokne type.
 * Refer to "token-list.h" to check token type.
 */
int scan(void) {
	switch (check_char(cbuf)) {
		case ALPHABET:
			return get_key_token();
			break;
		case NUMBER:
			return get_number_token();
			break;
		case SYMBOL:
			return get_symbol_token();
			break;
		case STRING:
			return get_string_token();
			break;
		case SEPARATOR:
			cbuf = get_char();
			return scan();
			break;
		case NEWLINE:
			return scan();
			break;
		case COMMENT:
			skip_comment();
			return scan();
			break;
		/* ERROR */
		case UNKNOWN:
		default:
			break;
	}

	return ERROR;
}

/* return line num */
int get_linenum(void){
	return line_num;
}

/* Termination processing */
void end_scan(void){
	if(fp == NULL){
		return;
	}
	fclose(fp);
}

#ifdef TEST
extern int get_separator_num(void){
	return separator_num;
}

extern int get_comment_num(void){
	return comment_num;
}
#endif

/* find a type of character type */
static enum CHAR_T check_char(int c){
	enum CHAR_T type;

	if(is_alphabet(c)) type = ALPHABET;
	else if(is_number(c)) type = NUMBER;
	else if(is_symbol(c)) type = SYMBOL;
	else if(is_string(c)) type = STRING;
	else if(is_separator(c)) type = SEPARATOR;
	else if(is_newline(c)) type = NEWLINE;
	else if(is_comment(c)) type = COMMENT;
	else type = UNKNOWN;

	return type;
}

/* is_***() function
 * '***' shows a type of character.
 */
static sbool is_alphabet(int c){
	sbool flag = SFALSE;

	switch (c) {
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
			flag = STRUE;
			break;
		default:
			break;
	}

	return flag;
}

static sbool is_number(int c){
	sbool flag = SFALSE;

	switch (c) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			flag = STRUE;
			break;
		default:
			break;
	}

	return flag;
}

static sbool is_symbol(int c){
	int flag = SFALSE;

	switch (c) {
		case '+':
		case '-':
		case '*':
		case '=':
		case '<':
		case '>':
		case '(':
		case ')':
		case '[':
		case ']':
		case '.':
		case ',':
		case ':':
		case ';':
			flag = STRUE;
			break;
		default:
			break;
	}

	return flag;
}

static sbool is_string(int c){
	int flag = SFALSE;

	switch (c) {
		case '\'':
			flag = STRUE;
			break;
		default:
			break;
	}

	return flag;
}

static sbool is_separator(int c){
	int flag = SFALSE;

	switch (c) {
		case ' ':
		case '\t':
			flag = STRUE;
			#ifdef TEST
			separator_num++;
			#endif
			break;
		default:
			break;
	}

	return flag;
}

static sbool is_newline(int c){
	int flag = SFALSE;

	switch (c) {
		case '\n':
			if( (cbuf = get_char()) == '\r' ){
				cbuf = get_char();
			}
			line_num++;
			flag = STRUE;
			break;
		case '\r':
			if( (cbuf = get_char()) == '\n' ){
				cbuf = get_char();
			}
			line_num++;
			flag = STRUE;
			break;
		default:
			break;
	}

	return flag;
}

static sbool is_comment(int c){
	int flag = SFALSE;

	switch (c) {
		case '/':
			if( (cbuf = get_char()) == EOF ){
				error("'/' is not symbol.");
				break;
			}
			if( cbuf != '*'){
				error("'/' is not symbol.");
				break;
			}
			flag = STRUE;
			comment_type = COMMENT_SLASH;
			#ifdef TEST
			comment_num++;
			#endif
			break;
		case '{':
			flag = STRUE;
			comment_type = COMMENT_BRACKET;
			#ifdef TEST
			comment_num++;
			#endif
			break;
		default:
			break;
	}

	return flag;
}

static void init_string_attr(void){
	int i;
	for( i=0; i < MAXSTRSIZE; i++){
		string_attr[i] = '\0';
	}
}

/* find a type of keys and return this token code.
 * if this is not found keys, return NAME token code
 */
static int get_key_token(void){
	int token_code = TNAME; /* initialize token_code */
	int count = 0;
	int length, tmp_len;
	int i;

	init_string_attr();

	while(count < MAXSTRSIZE){
		string_attr[count++] = cbuf;
		if( (cbuf = get_char()) == EOF ){
			break;
		}
		if( is_newline(cbuf) ||  is_separator(cbuf) || is_symbol(cbuf) || is_string(cbuf) ){
			break;
		}
	}

	if( count >= MAXSTRSIZE ){
		token_code = ERROR;
		error("string length is over");
		return token_code;
	}

	for( i=0; i < KEYWORDSIZE; i++){
		struct KEY tmp_key = get_keyword(i);
		length = (count > (tmp_len = strlen(tmp_key.keyword))) ? count : tmp_len;
		if( strncmp(string_attr, tmp_key.keyword, length) == 0){
			token_code = tmp_key.keytoken;
			break;
		}
	}

	return token_code;
}

/* get number and return NUMBER token code */
static int get_number_token(void){
	int token_code = TNUMBER;
	int count = 0;

	init_string_attr();

	while(count < MAXSTRSIZE){
		string_attr[count++] = cbuf;
		if( (cbuf = get_char()) == EOF ){
			break;
		}
		if( is_newline(cbuf) || is_separator(cbuf) || is_symbol(cbuf) || is_alphabet(cbuf) ){
			break;
		}
	}

	if( count >= MAXSTRSIZE ){
		token_code = ERROR;
		error("string length is over");
		return token_code;
	}

	num_attr = atoi(string_attr);

	if( num_attr > MAX_NUMBER ){
		token_code = ERROR;
		error("number range is over.");
	}

	return token_code;
}

/* find a type of symbols and return this token code */
static int get_symbol_token(void){
	int token_code = 0;

	switch (cbuf) {
		case '+':
			token_code = TPLUS;
			break;
		case '-':
			token_code = TMINUS;
			break;
		case '*':
			token_code = TSTAR;
			break;
		case '=':
			token_code = TEQUAL;
			break;
		case '<':
			token_code = TLE;
			cbuf = get_char();
			switch (cbuf) {
				case '>':
					token_code = TNOTEQ;
					break;
				case '=':
					token_code = TLEEQ;
					break;
			}
			break;
		case '>':
			token_code = TGR;
			if( (cbuf = get_char()) == '=' ){
				token_code = TGREQ;
			}
			break;
		case '(':
			token_code = TLPAREN;
			break;
		case ')':
			token_code = TRPAREN;
			break;
		case '[':
			token_code = TLSQPAREN;
			break;
		case ']':
			token_code = TRSQPAREN;
			break;
		case ':':
			token_code = TCOLON;
			if( (cbuf = get_char()) == '=' ){
				token_code = TASSIGN;
			}
			break;
		case '.':
			token_code = TDOT;
			break;
		case ',':
			token_code = TCOMMA;
			break;
		case ';':
			token_code = TSEMI;
			break;
		default:
			break;
	}

	if(!(token_code == TCOLON || token_code == TGR || token_code == TLE)){
		cbuf = get_char();
	}

	return token_code;

}

/* get string and return STRING token code */
static int get_string_token(void){
	int token_code = TSTRING;
	int count = 0;
	int flag = SFALSE;

	init_string_attr();

	if( (cbuf = get_char()) == EOF ){
		error("missing terminating '\'' character.");
		return ERROR;
	}

	while(count < MAXSTRSIZE){
		string_attr[count++] = cbuf;
		length++;
		if( cbuf == '\'' ){
			flag = STRUE;
		}
		if( (cbuf = get_char()) == EOF ){
			if( flag == STRUE ){
				string_attr[--count] = '\0';
				break;
			}else{
				error("missing terminating '\'' character.");
				return ERROR;
			}
		}
		if( flag == STRUE ){ /* Maybe string finish */
			if( cbuf != '\'' ){
				string_attr[--count] = '\0';
				break; /* string finish */
			}else{ /* cbuf is ' */
				flag = SFALSE;
				string_attr[count++] = cbuf;
				if( (cbuf = get_char()) == EOF ){
					error("missing terminating '\'' character.");
					return ERROR;
				}
				continue;
			}
		}
		if( cbuf == '\n' || cbuf == '\r' ){
			error("line don't break in string");
			return ERROR;
		}
	}

	if( count >= MAXSTRSIZE ){
		token_code = ERROR;
		error("string length is over");
	}

	return token_code;
}

/* skip comment out */
static void skip_comment(void){
	char end_char;

	if( comment_type == COMMENT_SLASH ){
		end_char = '*';
	}else{
		end_char = '}';
	}

	while( (cbuf = get_char()) != end_char){
		if( cbuf == EOF ){
			error("comment out is not closed.");
			return;
		}
		/* If there is newline character, line num counts up. */
		if( is_newline(cbuf) == STRUE ){
			/* After calling is_newline(), cbuf is pointed a character at newline.  */
			if( cbuf == end_char ){
				break;
			}
		}
	}

	if( (cbuf = get_char()) == EOF && comment_type != COMMENT_BRACKET ){
		error("comment out is not closed.");
		return;
	}

	if( comment_type == COMMENT_SLASH ){
		if( cbuf == '/' ){
			cbuf = get_char();
		}else{
			skip_comment();
		}
	}

	return;
}

static int file_getc(void){
	return fgetc(fp);
}

#ifdef TEST
static int array_getc(void){
	return get_buf_string();
}

void switch_getc(void){
	get_char = array_getc;
}
#endif
