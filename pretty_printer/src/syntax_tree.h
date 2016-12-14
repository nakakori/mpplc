#ifndef _SYNTAX_TREE_H_
#define _SYNTAX_TREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ll_parse.h"
#include "../../token_count/src/scan.h"
#include "../../cross_reference/src/symbol_table.h"
// #include "../../token_count/src/id-list.h"


struct _DATA {
    int token;
    ID *id_pointer;
    int *num_pointer;
    char *str_pointer;
};
typedef struct _DATA DATA;

struct _SYNTAX_TREE {
    DATA data;
    struct _SYNTAX_TREE *child;
    struct _SYNTAX_TREE *next;
};
typedef struct _SYNTAX_TREE SYNTAX_TREE;

enum _SRESULT {
    NOTFOUND,
    FOUND,
    SET
};
typedef enum _SRESULT sresult;

extern SYNTAX_TREE *synroot;

extern void init_syntree(void);
extern void init_node(void);
extern void register_syntree(int t);
extern void end_list_node(void);
extern void release_data(void);

#endif /* _SYNTAX_TREE_H_ */
