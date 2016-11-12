/* scan.h */
#ifndef _SCAN_H_
#define _SCAN_H_

#include "token-list.h"

#ifdef TEST
#include "../test_src/tc_driver.h"
#endif


#define MAXSTRSIZE 1024
#define MAX_NUMBER 32767
#define ERROR -1

enum _BOOL{
	SFALSE = 0,
	STRUE = 1
};
typedef enum _BOOL sbool;

/* The next charater's token type */
enum CHAR_T{
	UNKNOWN,
	ALPHABET,
	NUMBER,
	SYMBOL,
	STRING,
	SEPARATOR,
	NEWLINE,
	COMMENT,
};

extern int init_scan(char *filename);
extern int scan(void);
extern int num_attr;
extern char string_attr[MAXSTRSIZE];
extern int length;
extern int get_linenum(void);
extern void end_scan(void);
#ifdef TEST
extern int get_separator_num(void);
extern int get_comment_num(void);
extern void switch_getc(void);
#endif

#endif /* _SCAN_H_ */
