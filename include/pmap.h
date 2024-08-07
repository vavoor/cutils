#ifndef HEADER_a8854df4_9ab8_4694_a185_4689d10e8d73
#define HEADER_a8854df4_9ab8_4694_a185_4689d10e8d73

/*
 * Implementation of a hash map of pointers.
 */

typedef struct _Pair {
  const char* key;
  void* data;
} Pair;

struct _PMapElement {
  struct _PMapElement* next;
  Pair pair;
  int hash;
};

typedef struct _PMap {
  int count;
  struct _PMapElement* first;
  struct _PMapElement* last;
  int capacity;
  int (*hash_function)(const char* key);
  struct _PMapElement** hash_table;
} PMap;

/* Default hash function for strings */
int PMapDefaultStringHash(const char* s);

/* Default hash functions for pointers. This could be usedful when always the same
 * pointers to strings are used as keys.
 */
int PMapAddressAsHash(const char* s);

/*
 * Creates a new hash map. If map is not NULL, map is used as hash map, otherwise mempory
 * is allocated.
 * 
 * Example 1
 *   PMap m1;
 *   PMap m = PMapNew(&m1)
 *   assert(p == m1;
 * 
 * Example 2
 *   PMap* m2 = PMapNew(NULL);
 * 
 */
PMap* PMapNew(PMap* map);
PMap* PMapNewCustomHash(PMap* map, int (*hash_function)(const char* key));
void PMapClear(PMap* map, void (*deleter)(void* data));
int PMapCount(PMap* map);

/* Overwrites data associated with key. Returns previous data or NULL. */
void* PMapOverwrite(PMap* map, const char* key, void* data);

/*
 * Inserts data unless already present. Returns data associated with key.
 */
void* PMapInsert(PMap* map, const char* key, void* data);

int PMapExists(PMap* map, const char* key);

int PMapIterate(PMap* map, int (*it)(int i, const char* key, void* data, void* pt), void* pt);
Pair* PMapFirst(PMap* map, void** it);
Pair* PMapNext(PMap* map, void** it);
void* PMapGet(PMap* map, const char* key);
Pair* PMapGetPair(PMap* map, const char* key);

const char* Internalize(PMap* syms, const char* s);


#endif /* HEADER_a8854df4_9ab8_4694_a185_4689d10e8d73 */
