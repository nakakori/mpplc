/* ll_parse.h */
#ifndef _LL_PARSE_H_
#define _LL_PARSE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../token_count/src/token-list.h"
#include "../../token_count/src/scan.h"
#include "../../cross_reference/src/symbol_table.h"
#include "syntax_tree.h"


extern int init_ll_parse(char *filename);
extern void end_ll_parse(void);
extern int parse_program(void);
extern void create_errmes(char *mes);

#endif /* _LL_PARSE_H_ */
