#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

// #include "../../pretty_printer/src/ll_parse.h"
// #include "../../token_count/src/scan.h"

struct TYPE {
    int ttype;
    int arraysize;
    struct TYPE *elmtp;
    struct TYPE *paratp;
};

struct LINE {
    int ref_linenum;
    struct LINE *nextlinep;
};

struct _ID {
    char *name;
    char *procname;
    struct TYPE *itype;
    int ispara;
    int deflinenum;
    struct LINE *irefp;
    struct _ID *nextp; // hash chain list
};
typedef struct _ID ID;

struct _SYM_LIST {
    ID **tablep;
    char *procname;
    struct _SYM_LIST *nextp;
};
typedef struct _SYM_LIST SYM_LIST;

struct _UNDEFINE_LIST {
    ID *idp;
    struct _UNDEFINE_LIST *nextp;
};
typedef struct _UNDEFINE_LIST UNDEFINE_LIST;

#define LOCAL 0
#define GLOBAL 1

#define TPINT 1
#define TPCHAR 2
#define TPBOOL 3
#define TPARRAY 4
#define TPPROC 5

#define VARIABLE 0
#define PARAMETER 1

#define HASHSIZE 101

extern SYM_LIST *table_list;
extern int namecount;

extern void init_symtab(void);
extern void release_symtab(void);

extern void switch_scope(int s);
extern void switch_varorpara(int vp);
extern void end_scope(int scope);

extern ID *search_symtab(char *np);

extern int register_symtab(char *np);
extern int register_subpro(char *np);
extern void register_type(int type, int arraysize);
extern void register_param(void);

extern int reference_name(char *np);
extern int reference_subpro(char *np);

extern int get_hash(char *str);

extern int get_array_element_type(char *np);
extern int param_type(int c);

#endif /* _SYMBOL_TABLE_H_ */
