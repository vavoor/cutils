#ifndef HEADER_980ec707_9071_4a62_bb35_30212c45851e
#define HEADER_980ec707_9071_4a62_bb35_30212c45851e

typedef struct {
  void* dummy[4];
} LList;

/**
 * Creates a linked list for elements of size <element_size>. If <list> is
 * non-null, this memory is initialized as LList. If <list> is NULL, a
 * list is allocated with malloc. This memory needs to be released by the user.
 *
 * Returns the pointer to the initialized linked list.
 */
LList* LListCreate(LList* list, int element_size);

/**
 * Removes all elements from the linked list.
 */
void LListClear(LList* list);

/**
 * Returns the number of elements in the linked list.
 */
int LListLength(LList* list);

/**
 * Returns the element size, as specified during the creation of the linked list.
 */
int LListElementSize(LList* list);

/**
 * Appends <element> to the list. For this, <element_size> bytes are copied from
 * <element> into the list.
 */
void* LListAppend(LList* list, void* element);

typedef struct {
  void* dummy[2];
} LListIt;

/**
 * Creates an interator for the linked list.
 * Returns the pointer to the first element of the list.
 *
 * LListIt it;
 * for (LListFirst(list, &it); !LListEol(&it); LListNext(&it)) {
 *   LListGet(&it, &element)
 *   ...
 * }
 *
 * LList it;
 * ep = LListFirst(list, &it);
 * while (ep != NULL) {
 *   ...
 *   ep = LListNext(&it);
 * }
 */
void* LListFirst(LList* list, LListIt* it);

/**
 * Returns true if the iterator points beyond the end of the linked list.
 */
int LListEol(LListIt* it);

/**
 * Advantes the iterator and return a pointer to the next element in the list.
 */
void* LListNext(LListIt* it);

/**
 * Retrieve the data stored in the list. If <element> is non-null,
 * the element is copied.
 *
 * Returns a pointer to the element in the list.
 */
void* LListGet(LListIt* it, void* element);

/**
 * Apply the <operation> on each element in the <list> passing the index of the
 * element (0, 1, 2, ...), a pointer to the element in the list, and <pass_through>.
 *
 * If <operation> returns non-null, the iteration is aborted.
 */
typedef int (*LListOp)(int index, void* element, void* pass_through);
void LListForAll(LList* list, LListOp operation, void* pass_through);

#endif /* HEADER_980ec707_9071_4a62_bb35_30212c45851e */
