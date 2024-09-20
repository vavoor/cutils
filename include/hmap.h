#ifndef HEADER_8461ecbf_bbb3_458f_881a_1597a7934436
#define HEADER_8461ecbf_bbb3_458f_881a_1597a7934436

/**
 * This abstract data type implements a hash table that maps keys on
 * arbitrary values. Keys must be strings (const char*).
 *
 * Example
 * -------
 *
 * HMap map;
 * HMapCreate(&map, sizeof(double));
 *
 * double value;
 * value = 1.0;
 * HMapPut(&map, "one", &value);
 * value = 2.0;
 * HMapPut(&map, "two", &value);
 *
 * int i;
 * for (i = 0; i < HMapLength(&map); i++) {
 *   const char* key = HMapGetKey(&map, i);
 *   HMapGetValue(&map, i, &value);
 * }
 *
 * i = HMapFind(&map, "two", &value);
 * assert(i >= 0); // entry found
 * const char* key = HMapGetKey(&map, i);
 *
 * i = HMapFind(&map, "foo", NULL);
 * assert(i == -1); // entry not found
 */

typedef struct HMap {
  void* dummy[8];
} HMap;

typedef struct {
  const char* key;
  void* element;
} HMapPair;

typedef int (*HMapOp)(int index, const char* key, void* element, void* pass_through);

/**
 * Creates a hash map that can store elements of size <element_size> bytes.
 * If <map> is non-null, <map> is initialized as hash map. If <map> is null,
 * a hash map is allocated with malloc. Note that in this case, the hash map
 * must be freed by the user.
 * Returns a pointer to the initialized hash map, either <map> or the newly
 * allocated memory.
 *
 * Note that <element_size> can be zero.
 *
 * Examples:
 * HMap map, *pmap;
 * pmap = HMapCreate(&map, sizeof(struct Data)); // pmap points at map
 * pmap = HMapCreate(NULL, sizeof(struct Data)); // pmap points at allocated memory
 */
HMap* HMapCreate(HMap* map, int element_size);

/**
 * Deletes all entries from the hash map. Note that the <map> itself is not
 * freed.
 * With HMapClear, <free_op> is called for each element in the hash map.
 */
void HMapClear(HMap* map, HMapOp free_op, void* passthrough);
void HMapClear2(HMap* map);

/**
 * Returns the number of elements in <map>.
 */
int HMapLength(HMap* map);

/**
 * Inserts a key-value pair into the hash table. If there was already an
 * element in the hash map with the same key, it is copied to <previous>,
 * provided <previous> is non-null.
 * Returns the pointer to the element that has been inserted or overwritten.
 */
void* HMapPut(HMap* map, const char* key, void* element, void* previous);

/**
 * Inserts a key-value pair into the hash table unless the key is already present.
 * Returns the index of the inserted element or -1 if it has not been inserted.
 */
void* HMapPutUnlessPresent(HMap* map, const char* key, void* element);

/**
 * Looks up the element associated with <key> in the hash table.
 * Returns the index or -1 if not found. If found, <element> is filled
 * with value. If not found, <element> is left unchanged.
 */
void* HMapFind(HMap* map, const char* key, void* element);

typedef struct {
  void* dummy[2];
} HMapIt;

void* HMapFirst(HMap* map, HMapIt* it, HMapPair* pair);
void* HMapNext(HMapIt* it, HMapPair* pair);
void* HMapData(HMapIt* it, HMapPair* pair);
int HMapEol(HMapIt* it);

#endif /* HEADER_8461ecbf_bbb3_458f_881a_1597a7934436 */
