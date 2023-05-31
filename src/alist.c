#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "alist.h"

struct AList {
  int elements_count;
  int element_size;
  int capacity;
  void* elements;
};

struct Iterator {
  AList* list;
  int idx;
};

AList* AListCreate(AList* list, int element_size, int capacity)
{
  assert(sizeof(AList) >= sizeof(struct AList));
  assert(sizeof(AListIt) >= sizeof(struct Iterator));
  assert(element_size > 0);
  assert(capacity >= 0);

  struct AList* l = (struct AList*) list;

  if (l == NULL) {
    l = malloc(sizeof(struct AList));
    assert(l != NULL);
  }

  l->elements_count = 0;
  l->element_size = element_size;
  l->capacity = capacity;
  l->elements = NULL;

  if (l->capacity > 0) {
    l->elements = malloc(l->capacity * l->element_size);
  }

  return (AList*) l;
}

AList* AListMove(AList* target, AList* source)
{
  assert(source != NULL);
  struct AList* ls = (struct AList*) source;

  struct AList* lt = (struct AList*) AListCreate(target, ls->element_size, 0);
  *lt = *ls;

  ls->elements_count = 0;
  ls->capacity = 0;
  ls->elements = NULL;

  return (AList*) lt;
}

void AListClear(AList* list, AListOp free_op, void* pass_through)
{
  assert(list != NULL);
  struct AList* l = (struct AList*) list;

  if (free_op != NULL) {
    int i;
    for (i = 0; i < l->elements_count; i++) {
      void* p = l->elements + l->element_size * i;
      free_op(i, p, pass_through);
    }
  }

  free(l->elements);
  l->elements = NULL;

  l->elements_count = 0;
  l->capacity = 0;
}

void AListClear2(AList* list)
{
  AListClear(list, NULL, NULL);
}

void AListTruncate(AList* list, int n)
{
  assert(list != NULL);
  struct AList* l = (struct AList*) list;

  if (0 <= n && n < l->elements_count) {
    l->elements_count = n;
  }
  else if (n < 0 && l->elements_count + n >= 0) {
    l->elements_count += n;
  }
}

int AListLength(AList* list)
{
  assert(list != NULL);
  struct AList* l = (struct AList*) list;
  return l->elements_count;
}

int AListElementSize(AList* list)
{
  assert(list != NULL);
  struct AList* l = (struct AList*) list;
  return l->element_size;
}

void* AListAppend(AList* list, void* element)
{
  assert(list != NULL);
  struct AList* l = (struct AList*) list;

  if (l->elements_count == l->capacity) {
    if (l->capacity == 0) {
      l->capacity = 256;
    }
    else if (l->capacity < 2048) {
      l->capacity = (l->capacity + 256) / 256 * 256;
    }
    else {
      l->capacity += l->capacity / 2;
    }
    l->elements = realloc(l->elements, l->element_size * l->capacity);
  }

  void* p = l->elements + l->element_size * l->elements_count;
  l->elements_count++;
  if (element == NULL) {
    memset(p, 0, l->element_size);
  }
  else {
    memcpy(p, element, l->element_size);
  }

  return p;
}

static void* fetch_element(struct AList* l, int i)
{
  void* p = NULL;

  if (0 <= i && i < l->elements_count) {
    p = l->elements + l->element_size * i;
  }
  else if (i < 0 && l->elements_count + i >= 0) {
    p = l->elements + l->element_size * (l->elements_count + i);
  }
  return p;
}

void* AListGet(AList* list, int i, void* element)
{
  assert(list != NULL);

  struct AList* l = (struct AList*) list;
  void* p = fetch_element(l, i);

  if (element != NULL && p != NULL) {
    memcpy(element, p, l->element_size);
  }

  return p;
}

void* AListSet(AList* list, int i, void* element)
{
  assert(list != NULL);
  assert(element != NULL);

  struct AList* l = (struct AList*) list;
  void* p = fetch_element(l, i);

  if (p != NULL) {
    memcpy(p, element, l->element_size);
  }

  return p;
}

void* AListFirst(AList* list, AListIt* it)
{
  assert(list != NULL);
  assert(it != NULL);

  struct Iterator* i = (struct Iterator*) it;
  i->list = list;
  i->idx = 0;

  return AListGet(list, i->idx, NULL);
}

int ALIstEol(AListIt* it)
{
  assert(it != NULL);
  struct Iterator* i = (struct Iterator*) it;

  return i->idx >= AListLength(i->list);
}

void* AListNext(AList* it)
{
  assert(it != NULL);
  struct Iterator* i = (struct Iterator*) it;
  if (i->idx < AListLength(i->list)) {
    i->idx++;
  }
  return AListGet(i->list, i->idx, NULL);
}

void* AListData(AListIt* it, void* element)
{
  assert(it != NULL);
  struct Iterator* i = (struct Iterator*) it;
  return AListGet(i->list, i->idx, NULL);
}

void AListForAll(AList* list, AListOp operation, void* pass_through)
{
  int n = AListLength(list);
  int i;
  for (i = 0; i < n; i++) {
    if (operation(i, AListGet(list, i, NULL), pass_through)) {
      break;
    }
  }
}
