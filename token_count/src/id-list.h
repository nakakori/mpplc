/* id-list.h */
#ifndef _ID_LIST_H_
#define _ID_LIST_H_

#include "token-list.h"

struct ID {
	char *name;
	int count;
	struct ID *nextp;
};

extern void init_idtab(void);
extern struct ID *search_idtab(char *np);
extern void id_countup(char *np);
extern void print_idtab(void);
extern void release_idtab(void);

#endif /* _ID_LIST_H_ */
