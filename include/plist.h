#ifndef HEADER_73d4b2b3_da30_4b2d_ada3_dc6c63c5e1aa
#define HEADER_73d4b2b3_da30_4b2d_ada3_dc6c63c5e1aa

/*
 * Implementation of a linked list of pointers.
 */
struct PListElement {
  struct PListElement* next;
  void* data;
};

typedef struct _PList {
  int count;
  struct PListElement* first;
  struct PListElement* last;
  struct PListElement** index;
} PList;

/*
 * Initializes <list>. If <list> is NULL, a list is allocated on the heap
 * and must be deallocated by the user.
 * Returns <list> or the pointer to the newly allocated list.
 */
PList* PListNew(PList* list);

/*
 * Removes all elements from <list>. If <deleter> is not NULL, it will be called
 * for all elements in the list. E.g., free() can be used as <deleter>.
 */
void PListClear(PList* list, void (*deleter)(void* data));

/*
 * Appends the element <data> to the list. Note that only the pointer is copied.
 */
void PListAppend(PList* list, void* data);

/*
 * Inserts the element <data> at he beginning of <list>. Thus a list can be
 * used as stack.
 */
void PListPush(PList* list, void* data);

/*
 * Removes the first element from the list when using it as stack.
 * Returns the removed element or NULL if the list ist empty.
 */
void* PListPop(PList* list);

/*
 * Returns the first element of <list> or NULL is the liust is empty.
 */
void* PListTop(PList* list);

/*
 * Returns the numer of elements in the list.
 */
int PListCount(PList* list);

/*
 * Returns the n-th element in the list (counting starts at 0).
 * Note that an index is created for this operation!
 */
void* PListAt(PList* list, int n);

/*
 * Iterates through <list> and calls <it> for each element. <it> receives as argument
 * the index <i> of the element, the element <data>, and <pt>, which is passed through.
 * The iteration stops when <it> returns a non-null value.
 * Returns the return value of the last call to <it>, which is 0 if the iterator
 * does not abort.
 */
int PListIterate(PList* list, int (*it)(int i, void* data, void* pt), void* pt);

/*
 * Initializes the iterator <it> to reference the first element.
 *
 * struct Data* data;
 * void* it;
 * PListFirst(l, &it);
 * while (data = PListNext(&it)) {
 *   ... do something with data ...
 * }
 */
void* PListFirst(PList* list, void** it);

/*
 * Copies the element referenced by the iterator <it> to data (if non-null)
 * and advances the <it>.
 *
 * Returns a pointer to the internal data or NULL if end of list has been reached.
 */
void* PListNext(void** it);

#endif /* HEADER_73d4b2b3_da30_4b2d_ada3_dc6c63c5e1aa */
