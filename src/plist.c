#include <assert.h>
#include <stdlib.h>

#include "plist.h"

/*
 * Implementation of a linked list of pointers.
 */
struct _PListElement {
  struct _PListElement* next;
  void* data;
};

struct _PList {
  int count;
  struct _PListElement* first;
  struct _PListElement* last;
  struct _PListElement** index;
};

PList* PListCreate(PList* list)
{
  assert(sizeof(PList) >= sizeof(struct _PList));
  struct _PList* l = (struct _PList*)list;
  if (list == NULL) {
    l = malloc(sizeof(struct _PList));
    assert(l != NULL);
  }

  l->count = 0;
  l->first = l->last = NULL;
  l->index = NULL;

  return (PList*) l;
}

int PListCount(PList* list)
{
  assert(list != NULL);
  struct _PList* l = (struct _PList*)list;
  return l->count;
}

void PListAppend(PList* list, void* data)
{
  assert(list != NULL);
  assert(data != NULL);

  struct _PList* l = (struct _PList*) list;

  struct _PListElement* e = malloc(sizeof(struct _PListElement));
  assert(e != NULL);

  e->next = NULL;
  e->data = data;

  if (l->first == NULL) {
    l->first = l->last = e;
  }
  else {
    l->last->next = e;
    l->last = e;
  }

  if (l->index != NULL) {
    free(l->index);
    l->index = NULL;
  }

  l->count++;
}

void PListPush(PList* list, void* data)
{
  assert(list != NULL);
  assert(data != NULL);

  struct _PList* l = (struct _PList*) list;

  struct _PListElement* e = malloc(sizeof(struct _PListElement));
  assert(e != NULL);

  e->next = l->first;
  e->data = data;

  if (l->first == NULL) {
    l->first = l->last = e;
  }
  else {
    l->first = e;
  }

  if (l->index != NULL) {
    free(l->index);
    l->index = NULL;
  }

  l->count++;
}

void* PListPop(PList* list)
{
  assert(list != NULL);
  struct _PList* l = (struct _PList*) list;

  if (l->first != NULL) {
    struct _PListElement* e = l->first;
    l->first = e->next;
    if (l->first == NULL) {
      l->last = NULL;
    }

    if (l->index != NULL) {
      free(l->index);
      l->index = NULL;
    }
    l->count--;

    void* t = e->data;
    free(e);
    return t;
  }
  else {
    return NULL;
  }
}

void* PListTop(PList* list)
{
  void* it;
  return PListFirst(list, &it);
}

void PListClear(PList* list, void (*deleter)(void* data))
{
  assert(list != NULL);
  struct _PList* l = (struct _PList*) list;
  struct _PListElement* e = l->first;
  while (e != NULL) {
    struct _PListElement* n = e->next;
    if (deleter != NULL) {
      deleter(e->data);
    }
    free(e);
    e = n;
  }

  l->count = 0;
  l->first = l->last = NULL;

  if (l->index != NULL) {
    free(l->index);
    l->index = NULL;
  }
}

static void create_index(struct _PList* list)
{
  if (list->index == NULL) {
    list->index = malloc(list->count * sizeof(struct _PListElement*));
    assert(list->index != NULL);

    struct _PListElement* e = list->first;
    int i = 0;
    while (e != NULL) {
      struct _PListElement* n = e->next;
      list->index[i] = e;
      i++;
      e = n;
    }
  }
}

void* PListAt(PList* list, int idx)
{
  assert(list != NULL);
  struct _PList* l = (struct _PList*) list;

  create_index(l);
  return 0 <= idx && idx < l->count ? l->index[idx]->data : NULL;
}

void* PListSet(PList* list, int idx, void* data)
{
  assert(list != NULL);
  struct _PList* l = (struct _PList*) list;

  create_index(l);
  if (0 <= idx && idx < l->count) {
    void* previous = l->index[idx]->data;
    l->index[idx]->data = data;
    return previous;
  }
  else {
    return NULL;
  }
}

int PListIterate(PList* list, int (*it)(int i, void* data, void* pt), void* pt)
{
  assert(list != NULL);
  assert(it != NULL);
  struct _PList* l = (struct _PList*) list;

  int i = 0;
  struct _PListElement* e = l->first;
  while (e != NULL) {
    int ret;
    if ((ret = it(i, e->data, pt))) {
      return ret;
    }
    i++;
    e = e->next;
  }
  return 0;
}

void* PListFirst(PList* list, void** it)
{
  assert(list != NULL);
  assert(it != NULL);
  struct _PList* l = (struct _PList*) list;

  struct _PListElement* e = l->first;
  *it = e;

  return e != NULL ? e->data : NULL;
}

void* PListNext(void** it)
{
  assert(it != NULL);
  struct _PListElement* e = (struct _PListElement*) *it;

  if (e != NULL) {
    *it = e->next;
    return e->data;
  }

  return NULL;
}
