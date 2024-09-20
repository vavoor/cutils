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
 *   PMap m = PMapCreate(&m1)
 *   assert(p == m1;
 *
 * Example 2
 *   PMap* m2 = PMapCreate(NULL);
 *
 */
PMap* PMapCreate(PMap* map);
PMap* PMapCreateCustomHash(PMap* map, int (*hash_function)(const char* key));
void PMapClear(PMap* map, void (*deleter)(void* data));
int PMapCount(PMap* map);

/* Overwrites data associated with key. Returns previous data or NULL. */
void* PMapOverwrite(PMap* map, const char* key, void* data);

/*
 * Inserts data unless already present. Returns data associated with key.
 */
void* PMapInsert(PMap* map, const char* key, void* data);

/*
 * Returns true if key exists in map
 */
int PMapExists(PMap* map, const char* key);

/*
 * Calls function <it> for each pair in the map. key and data of the pair as well as <pt> is passed
 * to <it>. If <it> returns non-null, the iteration is aborted. Note that the pairs are visited in
 * the order of insertion.
 * Returns the return value of <it> or 0 if the complete map has been iterated.
 */
int PMapIterate(PMap* map, int (*it)(int i, const char* key, void* data, void* pt), void* pt);

/*
 * Initializes iterator <it> to point at the first pair of the map.
 * Returns the first pair of the map.
 *
 * Example:
 *
 * void* iterator;
 * Pair* pair;
 *
 * PMapFirst(map, &iterator);
 * while ((pair = PMapNext(map, &iterator)) != NULL) {
 *   ... do something with pair ...
 * }
 */
Pair* PMapFirst(PMap* map, void** it);

/*
 * Advances the iterator <it>.
 * Returns the pair at which the iterator pointed before haveing been advanced or NULL
 * if no pair is left.
 * See PMapFirst for an example.
 */
Pair* PMapNext(PMap* map, void** it);

/*
 * Returns the data associated with <key> or NULL if <key> is not present.
 */
void* PMapGet(PMap* map, const char* key);
/*
 * Returns the pair that is indexed by <key> or NULL if <key> is not present.
 */
Pair* PMapGetPair(PMap* map, const char* key);

/*
 * Stores a copy of <s> in the map <syms> using <s> as key if not present already.
 * This function can be used to store strings exactly once.
 * Returns the key of <s>.
 *
 * Example
 *
 * const char* s1 = Internalize(sysm, "hello"); // The two strings are not the same!
 * const char* s2 = Internalize(syms, "hello");
 * assert(s1 == s2); // Same pointer
 * assert(strcmp(s1, "hello") == 0);
 *
 */
const char* Internalize(PMap* syms, const char* s);

#endif /* HEADER_a8854df4_9ab8_4694_a185_4689d10e8d73 */
