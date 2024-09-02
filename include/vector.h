#ifndef HEADER_79d8f03c_ae67_4eaa_9bac_279e6fa2bb40
#define HEADER_79d8f03c_ae67_4eaa_9bac_279e6fa2bb40

typedef struct {
  void* dummy[4];
} Vector;

typedef void (*VectorOp)(int index, void* element, void* pass_through);

/**
 * Initializes <vec> to be used as vector. If <vec> is NULL, memory is allocated.
 * Note that in this case, the caller must ensure the memory is deallocated after use.
 * Returns a pointer to the initialized memory.
 */
Vector* VecCreate(Vector* vec);

/**
 * Deletes all elements from <vec>. On return,, <vec> has length 0 and a
 * capacity of 1.
 * If <del> is non-null, it is called for each element to, e.g., to deallocate
 * its memory. <pt> is passed through to <del>.
 */
void VecClear(Vector* vec, VectorOp del, void* pt);

/**
 * A convenience function to call VecClear without a delete operation.
 */
void VecClear2(Vector* vec);

/**
 * Reduces the size of <vec>. If n is >= i, n elements are left in <vec>,
 * i.e. elements n, n+1, ... are removed. If n is negative, |n| elements
 * are removed from <vec>. If <del> is non-null, it is called as delete
 * operation for each element to be removed. <pt> is passed through to <del>.
 */
void VecTruncate(Vector* vec, int n, VectorOp del, void* pt);

/**
 * A convenience function to call VecTruncate without a delete operation.
 */
void VecTruncate2(Vector* vec, int i);

/**
 * Returns the number of elements in <vec>.
 */
int VecLength(Vector* vec);

/**
 * Appends <element> to <vec>. Note that this is the only function to grow
 * a vector.
 */
void VecAppend(Vector* vec, void* element);

/**
 * Returns the n-th element of <vec> or NULL, if the index is out of range.
 * If n is negative, the index is counted from the end of the vector.
 */
void* VecGet(Vector* vec, int n);

/**
 * Sets the n-th element of <vec> to <element>. If <n> is negative, the index
 * is counted from the end of the vector.
 * Returns the previous element at this index or NULL if the index was out of
 * range.
 * Note that vectors can grow only when using function VecAppend.
 */
void* VecSet(Vector* vec, int n, void* element);

/**
 * Returns the address of the n-th element of <vec> or NULL, if the index is
 * out of range. If <n> is negative, the index is calculated from the end of
 * the vector.
 */
void** VecAt(Vector* vec, int n);

#endif /* HEADER_79d8f03c_ae67_4eaa_9bac_279e6fa2bb40 */
