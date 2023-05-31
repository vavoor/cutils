#ifndef HEADER_770a4049_20c5_4e21_8946_edb5f890c033
#define HEADER_770a4049_20c5_4e21_8946_edb5f890c033

/**
 * This abstract data type implements list. AList stands for array list.
 *
 * Example
 * -------
 *
 * AList list;
 * AListCreate(&list, sizeof(const char*), 0);
 *
 * AListAppend(&list, "foo");
 * AListAppend(&list, "bar");
 *
 * int i;
 * for (i = 0; i < AListLength(&list); i++) {
 *   const char* s;
 *   AListGet(&list, i, &s);
 *   printf("%d -> %s\n", i, s);
 * }
 * AListClear(&list);
 */


typedef struct {
  void* dummy[4];
} AList;

typedef int (*AListOp)(int index, void* element, void* pass_through);


/**
 * Creates an array list for elements of size <element_size> with an
 * initial capacity to hold <capacity> elements in the variable <list>
 * points to. If <list> is NULL, memory for the list is allocated. This
 * memory needs to be released by the client.
 *
 * Returns <list> or the newly allocated list.
 *
 * Examples:
 * AList list, *listp;
 * listp = AListCreate(&list, sizeof(struct Elem), 0);
 * listp = AListCreate(NULL, sizeof(int), 25);
 */
AList* AListCreate(AList* list, int element_size, int capacity);

/**
 * Moves the array list <source> to <target>. If <target> is NULL, a new
 * array list is created. <source> is an empty array list after the call.
 *
 * Returns <target> or the pointer to the newly created array list.
 */
AList* AListMove(AList* target, AList* source);

/**
 * Calls <free_op> for each element in the list, if non-NULL and truncates
 *  the length of <list> to length zero and releases internally
 * allocated memory.
 */
void AListClear(AList* list, AListOp free_op, void* pass_through);

/**
 * Truncates the length of <list> to length zero and releases internally
 * allocated memory.
 */
void AListClear2(AList* list);

/**
 * Truncates the length of <list> to length <n>, if <n> is positive and less
 * than the length of <list>. If <n> is negative, the length of <list> is
 * shortened by |<n>|. The capacity of <list> remains unchaned.
 *
 * Examples:
 * Assume AListLength(alist) == 5 before calling AListTruncate.
 * After AListTruncate(alist, 3), the length of alist is 3
 * After AListTruncate(alist, 10), the length of alist is unchanged (5)
 * After AListTruncate(alist, 0), the length of alist is 0.
 * After AListTruncate(alist, -1), the length of alist is 4.
 * After AListTruncate(alist, -6), the length of alist is unchanged (5).
 */
void AListTruncate(AList* list, int n);

/**
 * Returns the number of elements in <list>.
 */
int AListLength(AList* list);

/**
 * Returns the size of an element in the list.
 */
int AListElementSize(AList* list);

/**
 * Appends an element to the end of <list>. For this <element_size> (see AListCreate)
 * bytes are copied from <element> into the list. If <element> is NULL, <element_size> bytes
 * are set to 0.
 *
 * Returns a pointer to the list element.
 */
void* AListAppend(AList* list, void* element);

/**
 * Fetches the element at the specified position from <list>. If <i> is positive,
 * the position is counted from the beginning of the list. If <i> is negative,
 * the position is counted from the end of the list. If <element> is non-NULL,
 * the element is copied to the variable <element> points at.
 *
 * Returns the pointer to the list element that has been fetched.
 *
 * Examples:
 * Assume list has length 5 and contains these elements [0,1,2,3,4]:
 * AListGet(list, 0, ...) returns 0
 * AListGet(list, 4, ...) returns 4
 * AListGet(list, 5, ...) returns NULL
 * AListGet(list, -1, ...) returns 4
 * AListGet(list, -5, ...) returns 0
 * AListGet(list, -6, ...) returns NULL
 */
void* AListGet(AList* list, int i, void* element);

/**
 * Overwrites the element at the specified position in <list>. The length of
 * the list remains unchanged. If <i> is positive, the position is counted from
 * the beginning of the list. If <i> is negative, the position is counted from
 * the end of the list.
 *
 * Returns the pointer to the list element that has been inserted.
 *
 * For examples see AListGet.
 */
void* AListSet(AList* list, int i, void* element);

typedef struct {
  void* dummy[2];
} AListIt;

void* AListFirst(AList* list, AListIt* it);
int ALIstEol(AListIt* it);
void* AListNext(AList* it);
void* AListData(AListIt* it, void* element);

/**
 * Apply the <operation> on each element in the <list> passing the index of the
 * element (0, 1, 2, ...), a pointer to the element in the list, and <pass_through>.
 *
 * If <operation> returns non-null, the iteration is aborted.
 */
void AListForAll(AList* list, AListOp operation, void* pass_through);

#endif /* HEADER_770a4049_20c5_4e21_8946_edb5f890c033 */
