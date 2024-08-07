#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "pset.h"

static void set_capacity(PSet* s, int capacity)
{
  s->elements = realloc(s->elements, capacity * sizeof(void*));
  assert(s->elements != NULL);
  s->capacity = capacity;
}

static void grow(PSet* set)
{
  int capacity = set->capacity >= 1024 ? (set->capacity + (set->capacity >> 1)) : (set->capacity > 0 ? (set->capacity + set->capacity) : 32);
  set_capacity(set, capacity);
}

static void shift(PSet* set, int n)
{
  memmove(&set->elements[n+1], &set->elements[n], (set->count - n) * sizeof(void*));
}

static int find_slot(PSet* set, void* key, int* slot)
{
  assert(set != NULL);
  
  if (set->count == 0) {
    *slot = 0;
    return 0;
  }
  
  int l = 0;
  int r = set->count;
  int m = (l + r) / 2;
  
  while (m > l && set->elements[m] != key) {
    if (set->elements[m] < key) {
      r = m;
    }
    else {
      l = m;
    }
    m = (l + r) / 2;
  }
  
  *slot = m;
  return set->elements[m] == key;
}

PSet* PSetNew(PSet* set)
{
  if (set == NULL) {
    set = malloc(sizeof(struct _PSet));
    assert(set != NULL);
  }
  
  set->count = 0;
  set->capacity = 0;
  set->elements = NULL;
  
  return set;
}

void PSetClear(PSet* set)
{
  assert(set != NULL);
  
  set->count = 0;
  set->capacity = 0;
  free(set->elements);
  set->elements = NULL;
}

int PSetSize(PSet* set)
{
  assert(set != NULL);
  return set->count;
}

int PSetAdd(PSet* set, void* element)
{
  assert(set != NULL);
  assert(element != NULL);
  assert(set->count <= set->capacity);
  
  int slot;
  if (find_slot(set, element, &slot)) {
    return 1;
  }
  else {
    if (set->count >= set->capacity) {
      grow(set);
    }
    shift(set, slot);
    set->elements[slot] = element;
    set->count++;
    return 0;
  }
}

int PSetContains(PSet* set, void* element)
{
  assert(set != NULL);
  assert(element != NULL);
  
  int slot;
  return find_slot(set, element, &slot);
}

void PSetUnion(PSet* a, PSet* b, PSet* u)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(u != NULL);
  assert(a != u);
  assert(b != u);
  
  PSetClear(u);
  set_capacity(u, a->count + b->count);
  
  int ia = 0;
  int ib = 0;
  int iu = 0;
  
  while (ia < a->count && ib < b->count) {
    if (a->elements[ia] < b->elements[ib]) {
      u->elements[iu] = a->elements[ia];
      iu++;
      ia++;
    }
    else if (a->elements[ia] > b->elements[ib]) {
      u->elements[iu] = b->elements[ib];
      iu++;
      ib++;
    }
    else {
      u->elements[iu] = a->elements[ia];
      iu++;
      ia++;
      ib++;
    }
  }
  
  while (ia < a->count) {
    u->elements[iu] = a->elements[ia];
    iu++;
    ia++;
  }
  
  while (ia < b->count) {
    u->elements[iu] = a->elements[ib];
    iu++;
    ib++;
  }
  
  u->count = iu;
}

void PSetIntersect(PSet* a, PSet* b, PSet* u)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(u != NULL);
  assert(a != u);
  assert(b != u);
  
  PSetClear(u);
  int max = a->count > b->count ? a->count : b->count;
  set_capacity(u, max);
  
  int ia = 0;
  int ib = 0;
  int iu = 0;
  
  while (ia < a->count && ib < b->count) {
    if (a->elements[ia] < b->elements[ib]) {
      ia++;
    }
    else if (b->elements[ib] < a->elements[ia]) {
      ib++;
    }
    else {
      u->elements[iu] = a->elements[ia];
      iu++;
      ia++;
      ib++;
    }
  }
  u->count = iu;
}
