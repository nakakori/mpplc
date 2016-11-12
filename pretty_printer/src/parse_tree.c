#include "parse_tree.h"

SYNTAX_TREE *synroot; /* root of syntax tree */
static SYNTAX_TREE **curroot; /* current root */

static sresult search_curlist(SYNTAX_TREE *root);

/* ********************************
 * initilize syntax tree
 * this function call only one time
 * ******************************** */
void init_syntree(void){
    init_idtab();
    synroot = NULL;
    curroot = &synroot;
}
/* *********************************************
 * initilize syntax tree node
 * this function call every analysis processing
 * ********************************************* */
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

void register_syntree(int t){
    SYNTAX_TREE *p;

    if((p = (SYNTAX_TREE *)malloc(sizeof(SYNTAX_TREE))) == NULL){
        printf("can not malloc in init_synlist\n");
        return;
    }
    // printf("malloc :%d token\n", t);
    (p->data).token = t;
    if(t == TNAME){
        id_countup(string_attr);
        (p->data).id_pointer = search_idtab(string_attr);
    }else{
        (p->data).id_pointer = NULL;
    }
    p->child = NULL;
    p->next = NULL;

    if(*curroot == NULL){
        *curroot = p;
    }else{
        SYNTAX_TREE *q;
        q = *curroot;
        // printf("list:");
        while (q->next != NULL) {
            // printf("%d ", q->data.token);
            q = q->next;
        }
        // printf("%d %d\n", q->data.token, p->data.token);
        q->next = p;
    }
}

void end_list_node(void){
    search_curlist(synroot);
}

/* ******************************************
 * this function is searched one list
 * return value: SET, FOUND, NOTFOUND
 * argument: root(pointer) to be searched list
 * ******************************************* */

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

void release_syntree(void){
    release_idtab();
    /* free() */
}
