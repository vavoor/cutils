#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "pmap.h"

/*
 * Searches <key> with hash value <hv> in the hash table of <map>.
 * Return true if key is present . In this case, <index> returns the slot
 * in the hash table.
 */
static int find_slot(PMap* map, const char* key, int hv, int* index)
{
  assert(map != NULL && map->capacity > 0);
  assert(key != NULL);
  assert(index != NULL);

  struct _PMapElement* e;

  int n = hv % map->capacity;
  int i = n;
  while (i < map->capacity) {
    e = map->hash_table[i];
    if (e == NULL) {
      *index = i;
      return 0; // key is not present
    }
    else if (hv == e->hash && strcmp(key, e->pair.key) == 0) {
      *index = i;
      return 1; // key is present
    }
    i++;
  }

  i = 0;
  while (i < n) {
    e = map->hash_table[i];
    if (e == NULL) {
      *index = i;
      return 0; // key is not present
    }
    else if (hv == e->hash && strcmp(key, e->pair.key) == 0) {
      *index = i;
      return 1; // key is present
    }
    i++;
  }

  assert("Hash table is full!" == NULL);
  return 0;
}

static void grow(PMap* map)
{
  map->capacity = map->capacity > 1024 ? (map->capacity + (map->capacity >> 1)) : (map->capacity > 0 ? (map->capacity + map->capacity) : 32);
  free(map->hash_table);
  map->hash_table = calloc(map->capacity, sizeof(struct PMapElement*));
  assert(map->hash_table != NULL);

  struct _PMapElement* e = map->first;
  while (e != NULL) {
    int i;
    int is_present = find_slot(map, e->pair.key, e->hash, &i);
    assert(is_present == 0);
    map->hash_table[i] = e;
    e = e->next;
  }
}

int PMapDefaultStringHash(const char* s)
{
  assert(s != NULL);
  int h = 17;
  const char* p = s;
  while (*p != '\0') {
    int c = *p;
    h += (c << 5) + (c << 2) + c;
    p++;
  }

  return h < 0 ? -h : h;
}

int PMapAddressAsHash(const char* s)
{
  long h = (long) s;
  return (int) h;
}

PMap* PMapNew(PMap* map)
{
  return PMapNewCustomHash(map, PMapDefaultStringHash);
}

PMap* PMapNewCustomHash(PMap* map, int (*hash_function)(const char* key))
{
  assert(hash_function != NULL);

  if (map == NULL) {
    map = malloc(sizeof(PMap));
    assert(map != NULL);
  }

  map->count = 0;
  map->first = map->last = NULL;
  map->capacity = 0;
  map->hash_function = hash_function;
  map->hash_table = NULL;

  return map;
}

int PMapCount(PMap* map)
{
  assert(map != NULL);
  return map->count;
}

#define GROW(MAP) if (((MAP)->count + ((MAP)->count >> 3) >= (MAP)->capacity)) {\
    grow((MAP));\
  }


void* PMapOverwrite(PMap* map, const char* key, void* data)
{
  assert(map != NULL);
  assert(key != NULL);

  GROW(map);

  int hv = map->hash_function(key);
  int i;
  int is_present = find_slot(map, key, hv, &i);

  if (is_present) {
    struct _PMapElement* e = map->hash_table[i];
    void* old_data = e->pair.data;
    e->pair.data = data;
    return old_data;
  }
  else {
    struct _PMapElement* e = malloc(sizeof(struct _PMapElement));
    assert(e != NULL);

    e->next = NULL;
    e->hash = hv;
    e->pair.key = key;
    e->pair.data = data;

    if (map->first == NULL) {
      map->first = map->last = e;
    }
    else {
      map->last->next = e;
      map->last = e;
    }
    map->count++;
    map->hash_table[i] = e;
    return NULL;
  }
}

void* PMapInsert(PMap* map, const char* key, void* data)
{
  assert(map != NULL);
  assert(key != NULL);

  GROW(map);

  int hv = map->hash_function(key);
  int i;
  int is_present = find_slot(map, key, hv, &i);

  if (is_present) {
    struct _PMapElement* e = map->hash_table[i];
    return e->pair.data;
  }
  else {
    struct _PMapElement* e = malloc(sizeof(struct _PMapElement));
    assert(e != NULL);
    e->next = NULL;
    e->hash = hv;
    e->pair.key = key;
    e->pair.data = data;

    if (map->first == NULL) {
      map->first = map->last = e;
    }
    else {
      map->last->next = e;
      map->last = e;
    }
    map->count++;
    map->hash_table[i] = e;
    return e->pair.data;
  }
}

const char* Internalize(PMap* syms, const char* s)
{
  assert(syms != NULL);
  assert(s != NULL);

  GROW(syms);

  int hv = syms->hash_function(s);
  int i;
  int is_present = find_slot(syms, s, hv, &i);

  if (is_present) {
    struct _PMapElement* e = syms->hash_table[i];
    return e->pair.data;
  }
  else {
    struct _PMapElement* e = malloc(sizeof(struct _PMapElement));
    assert(e != NULL);
    e->next = NULL;
    e->hash = hv;
    e->pair.key = strdup(s);
    assert(e->pair.key != NULL);
    e->pair.data = (void*) e->pair.key;

    if (syms->first == NULL) {
      syms->first = syms->last = e;
    }
    else {
      syms->last->next = e;
      syms->last = e;
    }
    syms->count++;
    syms->hash_table[i] = e;
    return e->pair.data;
  }
}

void PMapClear(PMap* map, void (*deleter)(void* data))
{
  assert(map != NULL);

  struct _PMapElement* e = map->first;
  while (e != NULL) {
    struct _PMapElement* n = e->next;
    if (deleter != NULL) {
      deleter(e->pair.data);
    }
    free(e);
    e = n;
  }

  map->count = 0;
  map->capacity = 0;
  map->first = map->last = NULL;
  free(map->hash_table);
  map->hash_table = NULL;
}

int PMapIterate(PMap* map, int (*it)(int i, const char* key, void* data, void* pt), void* pt)
{
  assert(map != NULL);
  assert(it != NULL);

  int i = 0;
  struct _PMapElement* e = map->first;
  while (e != NULL) {
    int ret = it(i, e->pair.key, e->pair.data, pt);
    if (ret != 0) {
      return ret;
    }
    i++;
    e = e->next;
  }
  return 0;
}

int PMapExists(PMap* map, const char* key)
{
  Pair* p = PMapGetPair(map, key);
  return p != NULL;
}

Pair* PMapFirst(PMap* map, void** it)
{
  assert(map != NULL);
  assert(it != NULL);

  struct _PMapElement* e = map->first;
  *it = e;

  return e != NULL ? &e->pair : NULL;
}

Pair* PMapNext(PMap* map, void** it)
{
  assert(map != NULL);
  assert(it != NULL);
  struct _PMapElement* e = (struct _PMapElement*) *it;

  if (e != NULL) {
    *it = e->next;
    return &e->pair;
  }

  return NULL;
}

void* PMapGet(PMap* map, const char* key)
{
  Pair* p = PMapGetPair(map, key);
  return p != NULL ? p->data : NULL;
}

Pair* PMapGetPair(PMap* map, const char* key)
{
  assert(map != NULL);
  assert(key != NULL);

  if (map->capacity == 0) {
    return NULL;
  }
  
  int hv = map->hash_function(key);
  int i;
  if (find_slot(map, key, hv, &i)) {
    struct _PMapElement* e = map->hash_table[i];
    return &e->pair;
  }
  return NULL;
}
