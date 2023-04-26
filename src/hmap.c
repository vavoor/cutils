#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alist.h"
#include "hmap.h"

struct _HMap {
  AList elements;

  int hash_capacity;
  int* hash_map;
};

struct Element {
  const char* key;
  int hash;
  char value[0];
};

static int hash_function(const char* key)
{
  int h = 1873;
  const char* p = key;
  while (*p != '\0') {
    h = (h << 5) + (h << 2) + h + *p; /* 37 * h + p */
    p++;
  }
  return h < 0 ? -h : h;
}

static int find_free_slot(struct _HMap* m, int hash)
{
  int h = hash % m->hash_capacity;
  int i;
  for (i = h; i < m->hash_capacity; i++) {
    if (m->hash_map[i] == -1) {
      return i;
    }
  }
  for (i = 0; i < h; i++) {
    if (m->hash_map[i] == -1) {
      return i;
    }
  }
  assert("Hash map has no free slot for rehashing" == NULL);
}

static int find_slot(struct _HMap* m, const char* key, int hash, int* slot)
{
  int found = 0;
  int hops = 0;
  int h = hash % m->hash_capacity;
  int i;
  for (i = h; i < m->hash_capacity; i++) {
    hops++;
    if (m->hash_map[i] != -1) {
      struct Element* e = AListGet(&m->elements, m->hash_map[i], NULL);
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
    if (m->hash_map[i] != -1) {
      struct Element* e = AListGet(&m->elements, m->hash_map[i], NULL);
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
  m->hash_map = malloc(m->hash_capacity * sizeof(int));
  assert(m->hash_map != NULL);

  int i;
  for (i = 0; i < m->hash_capacity; i++) {
    m->hash_map[i] = -1;
  }

  int len = AListLength(&m->elements);
  for (i = 0; i < len; i++) {
    struct Element* e = AListGet(&m->elements, i, NULL);
    int slot = find_free_slot(m, e->hash);
    m->hash_map[slot] = i;
  }
}

static void grow(struct _HMap* m)
{
  int limit = m->hash_capacity / 2 + m->hash_capacity / 4 + m->hash_capacity / 8;
  if (AListLength(&m->elements) >= limit) {
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

  AListCreate(&m->elements, sizeof(struct Element) + element_size, 0);
  m->hash_capacity = 0;
  m->hash_map =  NULL;
}

void HMapClear(HMap* map)
{
  assert(map != NULL);
  struct _HMap* m = (struct _HMap*) map;
  int len = AListLength(&m->elements);
  int i;
  for (i = 0; i < len; i++) {
    struct Element* e = AListGet(&m->elements, i, NULL);
    free((void*)e->key);
  }
  AListClear(&m->elements);
  free(m->hash_map);
  m->hash_map = NULL;
  m->hash_capacity = 0;
}

int HMapLength(HMap* map)
{
  assert(map != NULL);
  struct _HMap* m = (struct _HMap*) map;
  return AListLength(&m->elements);
}

const char* HMapGetKey(HMap* map, int i)
{
  assert(map != NULL);
  struct _HMap* m = (struct _HMap*) map;
  struct Element* el = AListGet(&m->elements, i, NULL);
  return el->key;
}

void* HMapGetValue(HMap* map, int i, void* element)
{
  assert(map != NULL);
  struct _HMap* m = (struct _HMap*) map;
  struct Element* el = AListGet(&m->elements, i, NULL);

  if (element != NULL) {
    memcpy(element, &el->value, AListElementSize(&m->elements) - sizeof(struct Element));
  }

  return &el->value;
}

int HMapPut2(HMap* map, const char* key, void* element, int* overwritten)
{
  assert(map != NULL);
  assert(key != NULL);

  struct _HMap* m = (struct _HMap*) map;

  grow(m);

  int element_size = AListElementSize(&m->elements);
  int hash = hash_function(key);
  int slot;
  if (find_slot(m, key, hash, &slot)) {
    /* key is already in the map */
    struct Element* e = AListGet(&m->elements, m->hash_map[slot], NULL);
    if (element != NULL) {
      memcpy(&e->value, element, element_size - sizeof(struct Element));
    }
    *overwritten = 1;
  }
  else {
    /* key is not in the map */
    struct Element* e = alloca(element_size);
    e->key = strdup(key);
    e->hash = hash;
    if (element != NULL) {
      memcpy(&e->value, element, element_size - sizeof(struct Element));
    }
    int i = AListLength(&m->elements);
    AListAppend(&m->elements, e);
    m->hash_map[slot] = i;
    *overwritten = 0;
  }
  return m->hash_map[slot];
}

int HMapPut(HMap* map, const char* key, void* element)
{
  int overwritten;
  int index = HMapPut2(map, key, element, &overwritten);
  return index;
}

int HMapPutUnlessPresent(HMap* map, const char* key, void* element)
{
  assert(map != NULL);
  assert(key != NULL);

  struct _HMap* m = (struct _HMap*) map;

  grow(m);

  int element_size = AListElementSize(&m->elements);
  int hash = hash_function(key);
  int slot;
  if (find_slot(m, key, hash, &slot)) {
    /* key is already in the map */
    return -1;
  }
  else {
    /* key is not in the map */
    struct Element* e = alloca(element_size);
    e->key = strdup(key);
    e->hash = hash;
    if (element != NULL) {
      memcpy(&e->value, element, element_size - sizeof(struct Element));
    }
    int i = AListLength(&m->elements);
    AListAppend(&m->elements, e);
    m->hash_map[slot] = i;
    return m->hash_map[slot];
  }
}

int HMapFind(HMap* map, const char* key, void* element)
{
  assert(map != NULL);
  assert(key != NULL);

  struct _HMap* m = (struct _HMap*) map;
  int hash = hash_function(key);
  int slot;

  if (find_slot(m, key, hash, &slot)) {
    /* key is found */
    if (element != NULL) {
      struct Element* e = AListGet(&m->elements, m->hash_map[slot], NULL);
      memcpy(element, &e->value, AListElementSize(&m->elements) - sizeof(struct Element));
    }
    return m->hash_map[slot];
  }
  else {
    return -1;
  }
}
