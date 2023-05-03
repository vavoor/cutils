#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "llist.h"

struct Element {
    struct Element* next;
    char data[0];
};

struct _LList {
  int elements_count;
  int element_size;
  struct Element* first;
  struct Element* last;
};

struct Iterator {
  struct _LList* list;
  struct Element* element;
};

LList* LListCreate(LList* list, int element_size)
{
  assert(element_size > 0);
  assert(sizeof(LList) >= sizeof(struct _LList));
  assert(sizeof(LListIt) >= sizeof(struct Iterator));

  struct _LList* l = (struct _LList*) list;
  if (list == NULL) {
    l = malloc(sizeof(struct _LList));
    assert(l != NULL);
  }

  l->elements_count = 0;
  l->element_size = element_size;
  l->first = l->last = NULL;

  return (LList*) l;
}

void LListClear(LList* list)
{
  assert(list != NULL);
  struct _LList* l = (struct _LList*) list;

  struct Element* e = l->first;
  while (e != NULL) {
    struct Element* n = e->next;
    free(e);
    e = n;
  }

  l->elements_count = 0;
  l->first = l->last = NULL;
}

int LListLength(LList* list)
{
  assert(list != NULL);
  struct _LList* l = (struct _LList*) list;

  return l->elements_count;
}

int LListElementSize(LList* list)
{
  assert(list != NULL);
  struct _LList* l = (struct _LList*) list;

  return l->element_size;
}

void* LListAppend(LList* list, void* element)
{
  assert(list != NULL);
  struct _LList* l = (struct _LList*) list;

  struct Element* e = malloc(sizeof(struct Element) + l->element_size);
  if (element != NULL) {
    memcpy(e->data, element, l->element_size);
  }
  else {
    memset(e->data, 0, l->element_size);
  }
  e->next = NULL;

  if (l->last == NULL) {
    l->first = l->last = e;
  }
  else {
    l->last->next = e;
    l->last = e;
  }

  l->elements_count++;

  return e->data;
}

void* LListFirst(LList* list, LListIt* it)
{
  assert(list != NULL);
  assert(it != NULL);

  struct _LList* l = (struct _LList*) list;
  struct Iterator* i = (struct Iterator*) it;

  i->list = l;
  i->element = l->first;

  return i->element != NULL ? i->element->data : NULL;
}

int LListEol(LListIt* it)
{
  struct Iterator* i = (struct Iterator*) it;
  assert(i != NULL);
  assert(i->list != NULL);

  return i->element == NULL;
}

void* LListNext(LListIt* it)
{
  struct Iterator* i = (struct Iterator*) it;
  assert(i != NULL);
  assert(i->list != NULL);

  if (i->element != NULL) {
    i->element = i->element->next;
  }

  return i->element != NULL ? i->element->data : NULL;
}

void* LListGet(LListIt* it, void* element)
{
  struct Iterator* i = (struct Iterator*) it;
  assert(i != NULL);
  assert(i->list != NULL);

  if (element != NULL && i->element != NULL) {
    memcpy(element, i->element->data, i->list->element_size);
  }

  return i->element != NULL ? i->element->data : NULL;
}
