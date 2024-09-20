#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "llist.h"
#include "hmap.h"

struct Element {
  int hash;
  const char* key;
  char value[0];
};
#define ELEMENT_SIZE(N) (sizeof(struct Element) + (N))
#define DATA_SIZE(EL) ((EL) - sizeof(struct Element))

struct _HMap {
  LList elements;

  int hash_capacity;
  struct Element** hash_map; // Array of pointer to the elements in the list
};

static int hash_function(const char* key)
{
  int h = 1873;
  const char* p = key;
  while (*p != '\0') {
    h = (h << 5) + (h << 2) + h + *p; /* 37 * h + p */
    p++;
  }
  return 0x7FFFFFFF & h; // ensure the hash value is positive
}

static int find_free_slot(struct _HMap* m, int hash)
{
  assert(m->hash_capacity > 0);
  int h = hash % m->hash_capacity;
  int i;
  for (i = h; i < m->hash_capacity; i++) {
    if (m->hash_map[i] == NULL) {
      return i;
    }
  }
  for (i = 0; i < h; i++) {
    if (m->hash_map[i] == NULL) {
      return i;
    }
  }
  assert("Hash map has no free slot for rehashing" == NULL);
}

static int find_slot(struct _HMap* m, const char* key, int hash, int* slot)
{
  assert(m->hash_capacity > 0);

  int found = 0;
  int hops = 0;
  int h = hash % m->hash_capacity;
  int i;
  for (i = h; i < m->hash_capacity; i++) {
    hops++;
    struct Element* e = m->hash_map[i];
    if (e != NULL) {
      if (e->hash == hash && strcmp(key, e->key) == 0) {
        *slot = i;
        found = 1;
        goto exit;
      }
    }
    else {
      *slot = i;
      found = 0;
      goto exit;
    }
  }
  for (i = 0; i < h; i++) {
    hops++;
    struct Element* e = m->hash_map[i];
    if (e != NULL) {
      if (e->hash == hash && strcmp(key, e->key) == 0) {
        *slot = i;
        found = 1;
        goto exit;
      }
    }
    else {
      *slot = i;
      found = 0;
      goto exit;
    }
  }
  assert("Hash map has no free slot to insert element" == NULL);

exit:
  //~ fprintf(stderr, "# %d hops\n", hops);
  return found;
}

static void rehash(struct _HMap* m)
{
  //~ fprintf(stderr, "Rehashing (%d)\n", m->hash_capacity);
  m->hash_capacity = m->hash_capacity < 1024 ? 1024 : 2 * m->hash_capacity;
  free(m->hash_map);
  m->hash_map = calloc(m->hash_capacity, sizeof(struct Element*));
  assert(m->hash_map != NULL);

  LListIt it;
  struct Element* e = LListFirst(&m->elements, &it);
  while (e != NULL) {
    int slot = find_free_slot(m, e->hash);
    m->hash_map[slot] = e;
    e = LListNext(&it);
  }
}

static void grow(struct _HMap* m)
{
  int limit = m->hash_capacity / 2 + m->hash_capacity / 4 + m->hash_capacity / 8;
  if (LListLength(&m->elements) >= limit) {
    rehash(m);
  }
}

HMap* HMapCreate(HMap* map, int element_size)
{
  assert(sizeof(struct _HMap) <= sizeof(HMap));
  assert(element_size >= 0);

  struct _HMap* m = (struct _HMap*) map;
  if (map == NULL) {
    m = malloc(sizeof(struct _HMap));
    assert(m != NULL);
  }

  LListCreate(&m->elements, ELEMENT_SIZE(element_size));
  m->hash_capacity = 0;
  m->hash_map =  NULL;

  return (HMap*) m;
}

void HMapClear(HMap* map, HMapOp free_op, void* passthrough)
{
  assert(map != NULL);
  struct _HMap* m = (struct _HMap*) map;
  int i = 0;
  LListIt it;
  struct Element* e = LListFirst(&m->elements, &it);
  while (e != NULL) {
    if (free_op != NULL) {
      free_op(i, e->key, e->value, passthrough);
    }
    free((void*)e->key);
    i++;
    e = LListNext(&it);
  }
  LListClear2(&m->elements);
  free(m->hash_map);
  m->hash_map = NULL;
  m->hash_capacity = 0;
}

void HMapClear2(HMap* map)
{
  HMapClear(map, NULL, NULL);
}

int HMapLength(HMap* map)
{
  assert(map != NULL);
  struct _HMap* m = (struct _HMap*) map;
  return LListLength(&m->elements);
}

void* HMapPut(HMap* map, const char* key, void* element, void* previous)
{
  assert(map != NULL);
  assert(key != NULL);
  assert(element != NULL);

  struct _HMap* m = (struct _HMap*) map;

  grow(m);

  int element_size = LListElementSize(&m->elements);
  int hash = hash_function(key);

  int slot;
  if (find_slot(m, key, hash, &slot)) {
    /* key is already in the map */
    struct Element* e = m->hash_map[slot];
    if (previous != NULL) {
      memcpy(previous, e->value, DATA_SIZE(element_size));
    }
    memcpy(e->value, element, DATA_SIZE(element_size));
  }
  else {
    /* key is not in the map */
    struct Element* e = alloca(element_size);
    e->hash = hash;
    e->key = strdup(key);
    memcpy(e->value, element, DATA_SIZE(element_size));

    if (previous != NULL) {
      memset(previous, 0, DATA_SIZE(element_size));
    }

    m->hash_map[slot] = LListAppend(&m->elements, e);
  }
  return m->hash_map[slot]->value;
}

void* HMapPutUnlessPresent(HMap* map, const char* key, void* element)
{
  assert(map != NULL);
  assert(key != NULL);
  assert(element != NULL);

  struct _HMap* m = (struct _HMap*) map;

  grow(m);

  int element_size = LListElementSize(&m->elements);
  int hash = hash_function(key);

  int slot;
  if (!find_slot(m, key, hash, &slot)) {
    /* key is not in the map */
    struct Element* e = alloca(element_size);
    e->hash = hash;
    e->key = strdup(key);
    memcpy(e->value, element, DATA_SIZE(element_size));

    m->hash_map[slot] = LListAppend(&m->elements, e);
  }

  return m->hash_map[slot]->value;
}

void* HMapFind(HMap* map, const char* key, void* element)
{
  assert(map != NULL);
  assert(key != NULL);

  struct _HMap* m = (struct _HMap*) map;
  if (m->hash_capacity == 0) {
  	return NULL;
  }

  int hash = hash_function(key);
  int slot;
  if (find_slot(m, key, hash, &slot)) {
    /* key is found */
    struct Element* e = m->hash_map[slot];
    if (element != NULL) {
      memcpy(element, e->value, DATA_SIZE(LListElementSize(&m->elements)));
    }
    return e->value;
  }

  return NULL;
}

struct _HMapIt {
  LListIt lit;
};

void* HMapFirst(HMap* map, HMapIt* it, HMapPair* pair)
{
  assert(sizeof(struct _HMapIt) <= sizeof(HMapIt));
  assert(map != NULL);
  assert(it != NULL);

  struct _HMap* m = (struct _HMap*) map;
  struct _HMapIt* mit = (struct _HMapIt *) it;

  struct Element* e = LListFirst(&m->elements, &mit->lit);

  if (e != NULL) {
    if (pair != NULL) {
      pair->key = e->key;
      pair->element = e->value;
    }

    return e->value;
  }

  return NULL;
}

void* HMapNext(HMapIt* it, HMapPair* pair)
{
  assert(it != NULL);

  struct _HMapIt* mit = (struct _HMapIt *) it;
  struct Element* e = LListNext(&mit->lit);

  if (e != NULL) {
    if (pair != NULL) {
      pair->key = e->key;
      pair->element = e->value;
    }

    return e->value;
  }

  return NULL;
}

void* HMapData(HMapIt* it, HMapPair* pair)
{
  assert(it != NULL);
  struct _HMapIt* mit = (struct _HMapIt *) it;
  struct Element* e = LListData(&mit->lit, NULL);

  if (pair != NULL) {
    pair->key = e->key;
    pair->element = e->value;
  }

  return e->value;
}

int HMapEol(HMapIt* it)
{
  assert(it != NULL);
  struct _HMapIt* mit = (struct _HMapIt *) it;
  return LListEol(&mit->lit);
}
