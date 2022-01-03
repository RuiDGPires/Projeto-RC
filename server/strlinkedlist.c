#include "strlinkedlist.h"
#include <string.h>

sll_link_t sll_create(){
  return NULL;
}

void sll_destroy(sll_link_t *list){
  if (*list == NULL) return; 

  sll_destroy(&(*list)->next);
  free((*list)->data);
  free((*list));

  *list = NULL;
}

void sll_append(sll_link_t *list, const char *data){
  // Traverse until the end of the list
  if ((*list) == NULL){
    *list = (sll_link_t) malloc(sizeof(struct sll_node));
    (*list)->data = strdup(data);
    (*list)->next = NULL;
    (*list)->size = 1;
  }else if ((*list)->next == NULL){
    (*list)->next = (sll_link_t) malloc(sizeof(struct sll_node));
    (*list)->next->data = strdup(data);
    (*list)->next->next = NULL;
    (*list)->next->size = 1;
    (*list)->size += 1;
  }else {
    (*list)->size += 1;
    sll_append(&(*list)->next, data);
  }
}

void sll_push(sll_link_t *list, const char *data){
  if ((*list) == NULL){
    *list = (sll_link_t) malloc(sizeof(struct sll_node));
    (*list)->data = strdup(data);
    (*list)->next = NULL;
    (*list)->size = 1;
  }else{
    sll_link_t aux = *list;

    *list = (sll_link_t) malloc(sizeof(struct sll_node));
    (*list)->data = strdup(data);
    (*list)->next = aux;
    (*list)->size = aux->size + 1;
  }
}

size_t sll_size(sll_link_t list){
  return list->size;
}
