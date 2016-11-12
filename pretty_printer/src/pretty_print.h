/* pretty_print.h */

#ifndef _PRETTY_PRINT_H_
#define _PRETTY_PRINT_H_

#include "../../token_count/src/token-list.h"
#include "../../token_count/src/id-list.h"

#include "ll_parse.h"
#include "parse_tree.h"

enum _PBOOL{
    PFALSE = 0,
    PTRUE = 1
};
typedef enum _PBOOL pbool;

void pretty_print(SYNTAX_TREE *root);
void print_token(DATA data);
void print_indent(void);
void print_newline(void);
void print_space(void);

#endif /* _PRETTY_PRINT_H_ */
