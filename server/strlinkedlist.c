#include "strlinkedlist.h"
#include <string.h>
#include "../common/util.h"

sll_link_t sll_create(){
  return NULL;
}

void sll_destroy(sll_link_t *list){
  if (*list == NULL) return; 

  sll_destroy(&(*list)->next);
  free((*list)->str);
  free((*list));

  *list = NULL;
}

void sll_append(sll_link_t *list, const char *str){
  // Traverse until the end of the list
  if ((*list) == NULL){
    *list = (sll_link_t) malloc(sizeof(struct sll_node));
    (*list)->str = strdup(str);
    (*list)->next = NULL;
    (*list)->size = 1;
  }else if ((*list)->next == NULL){
    (*list)->next = (sll_link_t) malloc(sizeof(struct sll_node));
    (*list)->next->str = strdup(str);
    (*list)->next->next = NULL;
    (*list)->next->size = 1;
    (*list)->size += 1;
  }else {
    (*list)->size += 1;
    sll_append(&(*list)->next, str);
  }
}

void sll_append_ord(sll_link_t *list, const char *str, int (*ord)(const char *, const char *)){
  if ((*list) == NULL){
    //Empty
    *list = (sll_link_t) malloc(sizeof(struct sll_node));
    (*list)->str = strdup(str);
    (*list)->next = NULL;
    (*list)->size = 1;
  }else if (ord(str, (*list)->str) > 0){
    //Move on
    (*list)->size += 1;
    sll_append_ord(&(*list)->next, str, ord);
  }else{
    //Right place, current node needs to move
    sll_push(list, str);
  }
}

void sll_push(sll_link_t *list, const char *str){
  if ((*list) == NULL){
    *list = (sll_link_t) malloc(sizeof(struct sll_node));
    (*list)->str = strdup(str);
    (*list)->next = NULL;
    (*list)->size = 1;
  }else{
    sll_link_t aux = *list;

    *list = (sll_link_t) malloc(sizeof(struct sll_node));
    (*list)->str = strdup(str);
    (*list)->next = aux;
    (*list)->size = aux->size + 1;
  }
}

size_t sll_size(sll_link_t list){
    if (list == NULL) return 0;
    return list->size;
}
