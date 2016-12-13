#include "cross_refer.h"

static char *type_str[] = {"", "integer", "char", "boolean"};

void print_reference(char *var, ID **table);
struct NAME_LIST *make_namelist(void);
void sort_name_list(struct NAME_LIST *list);
int namecmp(const void *a, const void *b);
int stricmp(char *a, char *b);

int main(int argc, char *argv[]) {
    if(argc < 2){
        return error("File name id not given.");
    }

    if(init_ll_parse(argv[1]) < 0){
        return 0;
    }

    if(parse_program() < 0){
        return 0;
    }

    print_cross_reference(make_namelist());

    end_ll_parse();

    return 0;
}

void print_reference(char *var, ID **table){
    ID *id = table[get_hash(var)];
    if(id != NULL){
        while(id != NULL){
            if(strcmp(var, id->name) == 0){
                break;
            }
            id = id->nextp;
        }
        printf("%s", id->name); // print name

        if(id->procname != NULL){ // print procdure name
            printf(":%s", id->procname);
        }
        printf("\t");

        if(id->itype->ttype == TPPROC){
            printf("procedure");
            if(id->itype->paratp != NULL){
                printf("(");
                struct TYPE *p = id->itype->paratp;
                while (p != NULL) {
                    printf("%s", type_str[p->ttype]);
                    p = p->paratp;
                    if(p != NULL){
                        printf(", ");
                    }
                }
                printf(")");
            }
        }else if(id->itype->ttype == TPARRAY){
            printf("array [%d] of %s", id->itype->arraysize, type_str[id->itype->elmtp->ttype]);
        }else{
            printf("%s", type_str[id->itype->ttype]);
        }
        printf("\t");

        printf("%d | ", id->deflinenum);

        struct LINE *p = id->irefp;
        while(p != NULL){
            printf("%d", p->ref_linenum);
            p = p->nextlinep;
            if(p != NULL){
                printf(",");
            }
        }
        printf("\n");
    }
}

/* print cross reference */
void print_cross_reference(struct NAME_LIST *list){
    SYM_LIST *table = table_list;
    int i;

    // title
    printf("Name\tType\tDef.  Ref.\n");

    for(i = 0; i < namecount; i++){
        while(table != NULL){
            if(list[i].proc != NULL && table->procname != NULL){
                if(strcmp(list[i].proc,table->procname) == 0){
                    print_reference(list[i].name, table->tablep);
                    table = table_list;
                    break;
                }
            }else if(list[i].proc == NULL && table->procname == NULL){
                print_reference(list[i].name, table->tablep);
                table = table_list;
                break;
            }
            table = table->nextp;
        }
    }
}

/* create name list */
struct NAME_LIST *make_namelist(void){
    SYM_LIST *table;
    struct NAME_LIST *p;
    int i,j;

    if( (p = (struct NAME_LIST *)malloc(sizeof(struct NAME_LIST) * namecount)) == NULL ){
        printf("can not malloc in make_namelist()\n");
        return NULL;
    }

    table = table_list;

    for(i = 0; i < namecount;){
        if(table == NULL){
            break;
        }else{
            for(j = 0; j < HASHSIZE; j++){
                if(table->tablep[j] != NULL){
                    ID *id = table->tablep[j];
                    while(id != NULL){
                        p[i].name = id->name;
                        p[i].proc = id->procname;
                        i++;
                        id = id->nextp;
                    }
                }
            }
            table = table->nextp;
        }
    }

    sort_name_list(p);

    return p;
}

void sort_name_list(struct NAME_LIST *list){
    qsort((void *)list, namecount, sizeof(struct NAME_LIST), namecmp);
}

/* compare a and b */
int namecmp(const void *a, const void *b){
    struct NAME_LIST namea = *(struct NAME_LIST *)a;
    struct NAME_LIST nameb = *(struct NAME_LIST *)b;
    int result;

    if((result = stricmp(namea.name, nameb.name)) == 0){
        if(namea.proc != NULL && nameb.proc != NULL){
            return stricmp(namea.proc, nameb.proc);
        }else if(namea.proc != NULL && nameb.proc == NULL){
            return 1;
        }else if(namea.proc == NULL && nameb.proc != NULL){
            return -1;
        }
    }
    return result;
}

int stricmp(char *a, char *b){
    char abuf[MAXSTRSIZE] = {}, bbuf[MAXSTRSIZE] = {};
    int i;

    for(i = 0; i < (int)strlen(a); i++){
        abuf[i] = tolower(a[i]);
    }
    for(i = 0; i < (int)strlen(b); i++){
        bbuf[i] = tolower(b[i]);
    }

    return strcmp(abuf, bbuf);
}
