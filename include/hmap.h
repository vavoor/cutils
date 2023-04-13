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
 */
void HMapClear(HMap* map);

/**
 * Returns the number of elements in <map>.
 */
int HMapLength(HMap* map);

/**
 * Returns the key of the i-th element in the hash map. Note that the elements
 * are stored in the order of insertion.
 */
const char* HMapGetKey(HMap* map, int i);

/**
 * Returns the value of the i-th element in the hash map. Note that the elements
 * are stored in the order of insertion.
 * If <element> is non-null, the value is copied to the <element>.
 *
 * Returns a pointer to i-th element in the map.
 */
void* HMapGetValue(HMap* map, int i, void* element);

/**
 * Inserts a key-value pair into the hash table.
 * Returns 1 if the element has been inserted or 0 if it has been overwritten.
 */
int HMapPut(HMap* map, const char* key, void* element);

/**
 * Looks up the element associated with <key> in the hash table.
 * Returns the index or -1 if not found. If found, element is filled
 * with value.
 */
int HMapFind(HMap* map, const char* key, void* element);

#endif /* HEADER_8461ecbf_bbb3_458f_881a_1597a7934436 */
