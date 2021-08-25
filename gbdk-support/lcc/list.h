// list.h

#include <stdbool.h>

#ifndef _LCC_LIST_H
#define _LCC_LIST_H


typedef struct list *List;
struct list {       /* circular list nodes: */
    char *str;      /* option or file name */
    List link;      /* next list element */
};

List append(char *, List);
List find(char *, List);

List path2list(const char *);

void list_rewrite_exts(List, char *, char *);
void list_duplicate_to_new_exts(List, char *, char *);
List list_remove_all(List);
List list_add_to_another(List, List, char *, char *);


#endif // _LCC_LIST_H
