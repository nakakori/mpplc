#include "symbol_table.h"

#include "../../pretty_printer/src/ll_parse.h"
#include "../../token_count/src/scan.h"

SYM_LIST *table_list;
int namecount;

static ID **global_table, **local_table, **cur_table;
static int scope;
static int varorpara;
static UNDEFINE_LIST *undef_list, *param_list;
static int undefcount;
static ID *subpro, *call_subpro;

static void release_type(struct TYPE *tp);
static void release_line(struct LINE *lp);

static ID **create_hash_table(void);
static void set_hash_table(ID *newid);
static void stack_tablist(ID **table);
static void stack_undeflist(ID *newid, int param);
static char *str_alloc(char *str);
static int get_paramnum(void);

/* initialise symbol table module */
void init_symtab(void){
    table_list = NULL;
    namecount = 0;
    global_table = create_hash_table();
    local_table = NULL;
    cur_table = global_table;
    scope = GLOBAL;
    varorpara = VARIABLE;
    undef_list = NULL;
    param_list = NULL;
    undefcount = 0;
    subpro = NULL;
    call_subpro = NULL;
}

void release_symtab(void){
    int i;
    while(table_list != NULL){ // release symbol table list
        ID **table = table_list->tablep;
        for(i = 0; i < HASHSIZE; i++){ // release symbol table
            if(table[i] != NULL){
                free(table[i]->name);
                if(table[i]->procname){
                    free(table[i]);
                }
                release_type(table[i]->itype);
                release_line(table[i]->irefp);
                free(table[i]);
            }
        }
        SYM_LIST *tmp = table_list;
        table_list = table_list->nextp;
        if(tmp->procname != NULL){
            free(tmp->procname);
        }
        free(tmp);
    }
}

void release_type(struct TYPE *tp){
    struct TYPE *tmp;
    while(tp != NULL){
        tmp = tp;
        tp = tp->elmtp;
        free(tmp);
    }
}

void release_line(struct LINE *lp){
    struct LINE *tmp;
    while(lp != NULL){
        tmp = lp;
        lp = lp->nextlinep;
        free(tmp);
    }
}

void switch_scope(int s){
    scope = s;
    if(scope == LOCAL){
        local_table = create_hash_table();
        cur_table = local_table;
    }else{
        cur_table = global_table;
        subpro = NULL;
    }
}

void switch_varorpara(int vp){
    varorpara = vp;
}

void end_scope(int scope){
    if(scope == LOCAL){
        stack_tablist(local_table);
    }else{
        stack_tablist(global_table);
    }
}

/*
 * this function is called at register_syntree()
 */
ID *search_symtab(char *np){
    ID *id;

    if(scope == LOCAL && (id = local_table[get_hash(np)]) != NULL){
        id = local_table[get_hash(np)];
        if(id->nextp != NULL){
            while(id->nextp != NULL){
                if(strcmp(np, id->name) == 0){
                    return id;
                }
                id = id->nextp;
            }
        }
        if(strcmp(np, id->name) == 0){
            return id;
        }
        return NULL;
    }else if((id = global_table[get_hash(np)]) != NULL){ // global
        if(id->nextp != NULL){
            while(id->nextp != NULL){
                if(strcmp(np, id->name) == 0){
                    return id;
                }
                id = id->nextp;
            }
        }
        if(strcmp(np, id->name) == 0){
            return id;
        }
        return NULL;
    }
    return NULL;
}

/*
 * register symbol table.(only name)
 * this function is called at arrange_var(), sub_program()
 */
int register_symtab(char *np){
    ID *p;

    if((p = search_symtab(np)) != NULL){ // multi definition
        if(scope == GLOBAL || (scope == LOCAL && p->procname != NULL)){
            char buf[MAXSTRSIZE];
            sprintf(buf, "%s is multi definition", np);
            create_errmes(buf);
            return ERROR;
        }
    }
    // register
    if( (p = (ID *)malloc(sizeof(ID))) == NULL ){
        printf("can not malloc in register_symtab()\n");
        return ERROR;
    }

    p->name = str_alloc(np);
    if(scope == LOCAL){
        p->procname = str_alloc(subpro->name);
    }else { // scope == GLOBAL
        p->procname = NULL;
    }
    p->itype = NULL;
    p->ispara = varorpara;
    p->deflinenum = get_linenum();
    p->irefp = NULL;
    p->nextp = NULL;

    set_hash_table(p);
    stack_undeflist(p, VARIABLE);

    if(varorpara == PARAMETER){
        stack_undeflist(p, PARAMETER);
    }
    namecount++;
    return 0; // SUCCESS
}

int register_subpro(char *np){
    ID *p;

    if((p = search_symtab(np)) != NULL){ // multi definition
        if(scope == GLOBAL || (scope == LOCAL && p->procname != NULL)){
            char buf[MAXSTRSIZE];
            sprintf(buf, "%s is multi definition", np);
            create_errmes(buf);
            return ERROR;
        }
    }

    if( (p = (ID *)malloc(sizeof(ID))) == NULL ){
        printf("can not malloc in register_subpro()\n");
        return ERROR;
    }
    p->name = str_alloc(np);
    p->procname = NULL; // sub_program is at global scope
    p->itype = NULL; // none return value. never change
    p->ispara = 0; // not formal parameter. but variable?
    p->deflinenum = get_linenum();
    p->irefp = NULL;
    p->nextp = NULL;

    set_hash_table(p);
    stack_undeflist(p, VARIABLE);
    subpro = p;
    namecount++;
    register_type(TPPROC, 0);
    return 0;
}

/*
 * register type and define type of undefined name
 * @param (type) : standard_type (integer, char, boolean)
 * @param (arraysize) : 0(standard_type), else(array type)
 */
void register_type(int type, int arraysize){
    struct TYPE **p;
    int i;

    if( (p = (struct TYPE **)malloc(sizeof(struct TYPE *) * undefcount)) == NULL ){
        printf("can not malloc in register_type()\n");
        return;
    }
    for(i = 0; i < undefcount; i++){
        struct TYPE *tp;
        if( (tp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL ){
            printf("can not malloc in register_type()\n");
            return;
        }
        tp->ttype = type;
        tp->arraysize = 0;
        tp->elmtp = NULL;
        tp->paratp = NULL;
        p[i] = tp;
    }

    if(arraysize > 0){ // array type
        struct TYPE **q;

        if( (q = (struct TYPE **)malloc(sizeof(struct TYPE *) * undefcount)) == NULL ){
            printf("can not malloc in register_type()\n");
            return;
        }
        for(i = 0; i < undefcount; i++){
            struct TYPE *tq;
            if( (tq = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL ){
                printf("can not malloc in register_type()\n");
                return;
            }
            tq->ttype = TPARRAY;
            tq->arraysize = arraysize;
            tq->elmtp = p[i];
            tq->paratp = NULL;
            q[i] = tq;
            p[i] = q[i];
        }
    }

    while(undef_list != NULL){
        undef_list->idp->itype = p[--undefcount];
        UNDEFINE_LIST *tmp = undef_list;
        undef_list = tmp->nextp;
        free(tmp);
    }
    undefcount = 0; // 一応
}

void register_param(void){
    struct TYPE *p;

    if( (p = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL ){
        printf("can not malloc in register_type()\n");
        return;
    }
    p->ttype = TPPROC;
    p->arraysize = 0;
    p->elmtp = NULL;
    p->paratp = NULL;

    subpro->itype = p;

    while(param_list != NULL){
        UNDEFINE_LIST *tmp = param_list;
        param_list = tmp->nextp;
        if(param_list != NULL){
            param_list->idp->itype->paratp = tmp->idp->itype;
        }else{
            p->paratp = tmp->idp->itype;
        }
        free(tmp);
    }
}

/* hash functions */
ID **create_hash_table(void){
    ID **p = NULL;
    int i;

    if( (p = (ID **)malloc(sizeof(ID *) * HASHSIZE)) == NULL ){
        printf("can not malloc in register_symtab()\n");
        return NULL;
    }

    for(i = 0; i < HASHSIZE; i++){
        p[i] = NULL;
    }

    return p;
}

int get_hash(char *str){
    unsigned int h = 0;
    int i;
    int length = strlen(str);

    for (i = 0; i < length; i++) {
        h = h + str[i];
    }
    h = h % HASHSIZE;
    return h;
}

void set_hash_table(ID *newid){
    int hash = get_hash(newid->name);

    if(cur_table != NULL){
        if(cur_table[hash] == NULL){ // registered hash table
            cur_table[hash] = newid;
        }else{
            ID *p = cur_table[hash];
            while( p->nextp != NULL ){
                p = p->nextp;
            }
            p->nextp = newid;
        }
    }
}

void stack_tablist(ID **table){
    SYM_LIST *p;

    if( (p = (SYM_LIST *)malloc(sizeof(SYM_LIST))) == NULL ){
        printf("can not malloc in stack_tablist()\n");
        return;
    }

    p->tablep = table;
    if(scope == LOCAL){
        p->procname = str_alloc(subpro->name);
    }else{
        p->procname = NULL;
    }
    p->nextp = table_list;
    table_list = p;
}

void stack_undeflist(ID *newid, int param){
    UNDEFINE_LIST *p;
    UNDEFINE_LIST **list;

    if(param == VARIABLE){
        list = &undef_list;
        undefcount++;
    }else { // param == PARAMETER
        list = &param_list;
    }

    if( (p = (UNDEFINE_LIST *)malloc(sizeof(UNDEFINE_LIST))) == NULL ){
        printf("can not malloc in stack_undeflist()\n");
        return;
    }

    p->idp = newid;
    p->nextp = *list;
    *list = p;
}

char *str_alloc(char *str){
    char *p;

    if((p = (char *)malloc(strlen(str)+1)) == NULL) {
        printf("can not malloc in str_alloc\n");
    }
    strcpy(p, str);
    return p;
}

int reference_name(char *np){
    ID *p;

    if( (p = search_symtab(np)) == NULL){
        char buf[MAXSTRSIZE];
        sprintf(buf, "%s is undefined name\n", np);
        create_errmes(buf);
        return ERROR;
    }else{
        struct LINE *q, **tmp;
        if( (q = (struct LINE *)malloc(sizeof(struct LINE))) == NULL){
            char buf[MAXSTRSIZE];
            sprintf(buf, "can not malloc in reference_name()\n");
            create_errmes(buf);
            return ERROR;
        }
        q->ref_linenum = get_linenum();
        q->nextlinep = NULL;
        tmp = &(p->irefp);
        while(*tmp != NULL){
            tmp = &((*tmp)->nextlinep);
        }
        *tmp = q;
    }
    return p->itype->ttype;
}

int reference_subpro(char *np){
    ID *p;
    int tmpscope = scope;

    scope = GLOBAL;

    if( (p = search_symtab(np)) == NULL){
        char buf[MAXSTRSIZE];
        sprintf(buf, "%s is undefined sub program", np);
        create_errmes(buf);
        return ERROR;
    }else{
        if( p == subpro ){
            char buf[MAXSTRSIZE];
            sprintf(buf, "dont allow recursive call");
            create_errmes(buf);
            return ERROR;
        }
        struct LINE *q, **tmp;
        if( (q = (struct LINE *)malloc(sizeof(struct LINE))) == NULL){
            printf("can not malloc in reference_subpro()\n");
            return ERROR;
        }
        q->ref_linenum = get_linenum();
        q->nextlinep = NULL;
        tmp = &(p->irefp);
        while(*tmp != NULL){
            tmp = &((*tmp)->nextlinep);
        }
        *tmp = q;
    }

    call_subpro = p;

    scope = tmpscope;
    return get_paramnum();
}

int get_paramnum(void){
    int count = 0;
    struct TYPE *p = call_subpro->itype->paratp;

    while(p != NULL){
        count++;
        p = p->paratp;
    }

    return count;
}

int get_array_element_type(char *np){
    ID *p;
    if((p = search_symtab(np)) == NULL){
        return ERROR;
    }
    return p->itype->elmtp->ttype;
}

int param_type(int c){
    struct TYPE *p = call_subpro->itype;
    int i;

    if(c > get_paramnum()){
        return ERROR;
    }

    for(i = 0; i < c; i++){
        p = p->paratp;
    }
    return p->ttype;
}
