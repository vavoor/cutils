#include <assert.h>
#include <stdlib.h>

#include "plist.h"

PList* PListNew(PList* list)
{
  PList* l = list;
  if (list == NULL) {
    l = malloc(sizeof(struct _PList));
    assert(l != NULL);
  }

  l->count = 0;
  l->first = l->last = NULL;
  l->index = NULL;

  return l;
}

int PListCount(PList* list)
{
  assert(list != NULL);
  return list->count;
}

void PListAppend(PList* list, void* data)
{
  assert(list != NULL);
  assert(data != NULL);

  struct PListElement* e = malloc(sizeof(struct PListElement));
  assert(e != NULL);

  e->next = NULL;
  e->data = data;

  if (list->first == NULL) {
    list->first = list->last = e;
  }
  else {
    list->last->next = e;
    list->last = e;
  }

  if (list->index != NULL) {
    free(list->index);
    list->index = NULL;
  }

  list->count++;
}

void PListPush(PList* list, void* data)
{
  assert(list != NULL);
  assert(data != NULL);

  struct PListElement* e = malloc(sizeof(struct PListElement));
  assert(e != NULL);

  e->next = list->first;
  e->data = data;

  if (list->first == NULL) {
    list->first = list->last = e;
  }
  else {
    list->first = e;
  }

  if (list->index != NULL) {
    free(list->index);
    list->index = NULL;
  }

  list->count++;
}

void* PListPop(PList* list)
{
  assert(list != NULL);
  if (list->first != NULL) {
    struct PListElement* e = list->first;
    list->first = e->next;
    if (list->first == NULL) {
      list->last = NULL;
    }

    if (list->index != NULL) {
      free(list->index);
      list->index = NULL;
    }
    list->count--;

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
  struct PListElement* e = list->first;
  while (e != NULL) {
    struct PListElement* n = e->next;
    if (deleter != NULL) {
      deleter(e->data);
    }
    free(e);
    e = n;
  }

  list->count = 0;
  list->first = list->last = NULL;

  if (list->index != NULL) {
    free(list->index);
    list->index = NULL;
  }
}

void* PListAt(PList* list, int idx)
{
  assert(list != NULL);

  if (list->index == NULL) {
    list->index = malloc(list->count * sizeof(struct PListElement*));
    assert(list->index != NULL);

    struct PListElement* e = list->first;
    int i = 0;
    while (e != NULL) {
      struct PListElement* n = e->next;
      list->index[i] = e;
      i++;
      e = n;
    }
  }

  return 0 <= idx && idx < list->count ? list->index[idx]->data : NULL;
}

int PListIterate(PList* list, int (*it)(int i, void* data, void* pt), void* pt)
{
  assert(list != NULL);
  assert(it != NULL);

  int i = 0;
  struct PListElement* e = list->first;
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

  struct PListElement* e = list->first;
  *it = e;

  return e != NULL ? e->data : NULL;
}

void* PListNext(void** it)
{
  assert(it != NULL);
  struct PListElement* e = (struct PListElement*) *it;

  if (e != NULL) {
    *it = e->next;
    return e->data;
  }

  return NULL;
}
