#ifndef HEADER_79d8f03c_ae67_4eaa_9bac_279e6fa2bb40
#define HEADER_79d8f03c_ae67_4eaa_9bac_279e6fa2bb40

typedef struct {
  void* dummy[4];
} Vector;

typedef int (*VectorOp)(int index, void* element, void* pass_through);

Vector* VecCreate(Vector* vec);
void VecClear(Vector* vec, VectorOp del, void* pt);
void VecClear2(Vector* vec);
void VecTruncate(Vector* vec, int i, VectorOp del, void* pt);
void VecTruncate2(Vector* vec, int i);

int VecLength(Vector* vec);
void VecSetSize(Vector* vec, int size);
void VecAppend(Vector* vec, void* element);
void* VecGet(Vector* vec, int i);
void VecSet(Vector* vec, int i, void* element);

#endif /* HEADER_79d8f03c_ae67_4eaa_9bac_279e6fa2bb40 */
