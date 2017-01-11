#include "syntax_tree.h"

SYNTAX_TREE *synroot; /* root of syntax tree */
static SYNTAX_TREE **curroot; /* current root */

static sresult search_curlist(SYNTAX_TREE *root);
static void release_syntree(SYNTAX_TREE *root);
static int *num_alloc();
static char *str_alloc();

/*
 * initilize syntax tree
 * this function call only one time
 */
void init_syntree(void){
    // init_idtab();
    synroot = NULL;
    curroot = &synroot;
}
/*
 * initilize syntax tree node
 * this function call every analysis processing
 */
void init_node(void){
    SYNTAX_TREE *p;

    if(synroot != NULL){
        register_syntree(0); // empty node
        //print_synlist();
        p = *curroot;
        while(p->next != NULL){
            p = p->next;
        }
        curroot = &(p->child);
    }
    return;
}

SYNTAX_TREE *register_syntree(int t){
    SYNTAX_TREE *p;

    if((p = (SYNTAX_TREE *)malloc(sizeof(SYNTAX_TREE))) == NULL){
        printf("can not malloc in init_synlist\n");
        return NULL;
    }
    (p->data).token = t;
    if(t == TNAME){
        p->data.id_pointer = search_symtab(string_attr);
        p->data.num_pointer = NULL;
        p->data.str_pointer = NULL;
    }else if(t == TNUMBER){
        p->data.id_pointer = NULL;
        p->data.num_pointer = num_alloc();
        p->data.str_pointer = str_alloc();
    }else if(t == TSTRING){
        p->data.id_pointer = NULL;
        p->data.num_pointer = NULL;
        p->data.str_pointer = str_alloc();
    }else{
        p->data.id_pointer = NULL;
        p->data.num_pointer = NULL;
        p->data.str_pointer = NULL;
    }
    p->child = NULL;
    p->next = NULL;

    if(*curroot == NULL){
        *curroot = p;
    }else{
        SYNTAX_TREE *q;
        q = *curroot;
        while (q->next != NULL) {
            q = q->next;
        }
        q->next = p;
    }
    return p;
}

void end_list_node(void){
    search_curlist(synroot);
}

/*
 * this function is searched one list
 * return value: SET, FOUND, NOTFOUND
 * argument: root(pointer) to be searched list
 */

sresult search_curlist(SYNTAX_TREE *root){
    SYNTAX_TREE *p;
    sresult flag;
    p = root;

    while(p != NULL){ // if p is NULL pointer, a list is finished
        if(p->child == NULL){ // look for a node having child node
            p = p->next;
        }else{
            if(p->child == *curroot){ // match
                if(root == synroot){
                    curroot = &(synroot);
                    return SET;
                }else{
                    return FOUND;
                }
            }else{
                if((flag = search_curlist(p->child)) == FOUND){
                    curroot = &(p->child);
                    return SET;
                }else if(flag == SET){
                    return SET;
                }else{ // no match
                    p = p->next;
                }
            }
        }
    }

    return NOTFOUND;
}

void release_data(void){
    // release_idtab();
    release_symtab();
    release_syntree(synroot);
}

void release_syntree(SYNTAX_TREE *root){
    SYNTAX_TREE *p, *q;
    p = root;

    while(p != NULL){
        if(p->child != NULL){
            release_syntree(p->child);
        }
        if(p->data.num_pointer != NULL){
            free(p->data.num_pointer);
        }
        if(p->data.str_pointer != NULL){
            free(p->data.str_pointer);
        }
        q = p->next;
        free(p);
        p = q;
    }
}

static int *num_alloc(){
    int *p;

    if((p = (int *)malloc(sizeof(int))) == NULL) {
        printf("can not malloc in num_alloc\n");
    }
    *p = num_attr;
    return p;
}

static char *str_alloc(){
    char *p;

    if((p = (char *)malloc(strlen(string_attr)+1)) == NULL) {
        printf("can not malloc in str_alloc\n");
    }
    strcpy(p, string_attr);
    return p;
}
