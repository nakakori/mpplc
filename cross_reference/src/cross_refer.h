#ifndef _CROSS_RIFER_H_
#define _CROSS_RIFER_H_

// #define TEST

#include <stdlib.h>
#include <ctype.h>

#include "symbol_table.h"

// #ifndef TEST
#include "../../pretty_printer/src/ll_parse.h"
// #endif

struct NAME_LIST {
    char *name;
    char *proc;
    // struct NAME_LIST *nextp;
};

void print_cross_reference(struct NAME_LIST *list);

#endif /* _CROSS_RIFER_H_ */
