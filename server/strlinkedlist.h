#ifndef STR_LINKED_LIST_H__
#define STR_LINKED_LIST_H__

#include <stdlib.h>

typedef struct sll_node *sll_link_t;
struct sll_node{
  char *str;
  size_t size;
  sll_link_t next;
};

sll_link_t sll_create();
void sll_destroy(sll_link_t *);
void sll_append(sll_link_t *, const char *);
void sll_push(sll_link_t *, const char *);
size_t sll_size(sll_link_t);

#define FOR_ITEM_IN_LIST(item, lst) {\
  for(sll_link_t aux = lst; aux != NULL; aux = aux->next){\
    item = aux->str;

#define END_FIIL() }}


#endif
