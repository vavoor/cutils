#ifndef HEADER_79d8f03c_ae67_4eaa_9bac_279e6fa2bb40
#define HEADER_79d8f03c_ae67_4eaa_9bac_279e6fa2bb40

typedef struct {
  void* dummy[2];
} PVector;

typedef void (*VectorOp)(int index, void* element, void* pass_through);

/**
 * Initializes <vec> to be used as pvector. If <vec> is NULL, memory is allocated.
 * Note that in this case, the caller must ensure the memory is deallocated after use.
 * Returns a pointer to the initialized memory.
 */
PVector* PVecCreate(PVector* vec);

/**
 * Deletes all elements from <vec>. On return,, <vec> has length 0 and a
 * capacity of 1.
 * If <del> is non-null, it is called for each element to, e.g., to deallocate
 * its memory. <pt> is passed through to <del>.
 */
void PVecClear(PVector* vec, VectorOp del, void* pt);

/**
 * A convenience function to call PVecClear without a delete operation.
 */
void PVecClear2(PVector* vec);

/**
 * Reduces the size of <vec>. If n is >= i, n elements are left in <vec>,
 * i.e. elements n, n+1, ... are removed. If n is negative, |n| elements
 * are removed from <vec>. If <del> is non-null, it is called as delete
 * operation for each element to be removed. <pt> is passed through to <del>.
 */
void PVecTruncate(PVector* vec, int n, VectorOp del, void* pt);

/**
 * A convenience function to call PVecTruncate without a delete operation.
 */
void PVecTruncate2(PVector* vec, int i);

/**
 * Returns the number of elements in <vec>.
 */
int PVecLength(PVector* vec);

/**
 * Appends <element> to <vec>. Note that this is the only function to grow
 * a pvector.
 */
void PVecAppend(PVector* vec, void* element);

/**
 * Returns the n-th element of <vec> or NULL, if the index is out of range.
 * If n is negative, the index is counted from the end of the pvector.
 */
void* PVecGet(PVector* vec, int n);

/**
 * Sets the n-th element of <vec> to <element>. If <n> is negative, the index
 * is counted from the end of the pvector.
 * Returns the previous element at this index or NULL if the index was out of
 * range.
 * Note that vectors can grow only when using function PVecAppend.
 */
void* PVecSet(PVector* vec, int n, void* element);

/**
 * Returns the address of the n-th element of <vec> or NULL, if the index is
 * out of range. If <n> is negative, the index is calculated from the end of
 * the pvector.
 */
void** PVecAt(PVector* vec, int n);

#endif /* HEADER_79d8f03c_ae67_4eaa_9bac_279e6fa2bb40 */
