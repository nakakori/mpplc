/* ll_parse.h */
#ifndef _LL_PARSE_H_
#define _LL_PARSE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../mppl_compiler/src/mpplc.h"
#include "../../token_count/src/token-list.h"
#include "../../token_count/src/scan.h"
#include "../../cross_reference/src/symbol_table.h"
#include "syntax_tree.h"

#define LEFT_VAR 0
#define RIGHT_VAR 1
#define ARG_VAR 2

struct LABELS {
    char *label;
    struct LABELS *next;
};

extern int init_ll_parse(char *filename);
extern void end_ll_parse(void);
extern int parse_program(void);
extern void create_errmes(char *mes);

#endif /* _LL_PARSE_H_ */
