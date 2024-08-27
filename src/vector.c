#include <assert.h>
#include <stdlib.h>

#include "vector.h"

struct _Vector {
  int length;
  void** elements;
};

static int normalize_index(struct _Vector* v, int i)
{
    int n;
    
    if (i < 0) {
      n = v->length + i;
    }
    else {
      n = i;
    }
    
    if (0 <= n && n < v->length) {
      return n;
    }
    else {
      return -1;
    }
}

static void** get_slot(struct _Vector* v, int i)
{
  if (v->length <= 0) {
    return NULL;
  }
  else if (v->length <= 256) {
    int i0 = i;
    void** p = v->elements;
    return &p[i0];
  }
  else if (v->length <= 256*256) {
    int i0 = i & 0x00FF;
    int i1 = i >> 8;
    void** p = v->elements;
    p = p[i1];
    return &p[i0];
  }
  else if (v->length <= 256*256*256) {
    int i0 = i & 0x00FF;
    int i1 = (i >> 8) & 0x00FF;
    int i2 = i >> 16;
    void** p = v->elements;
    p = p[i2];
    p = p[i1];
    return &p[i0];
  }
  else {
    return NULL;
  }
}

Vector* VectorCreate(Vector* vec)
{
  assert(sizeof(Vector) >= sizeof(struct _Vector));
  struct _Vector* v = (struct _Vector*) vec;
  
  if (v == NULL) {
    v = malloc(sizeof(struct _Vector));
    assert(v != NULL);
  }
  
  v->length = 0;
  v->elements = NULL;
  
  return (Vector*) v;
}

int VectorLength(Vector* vec)
{
  assert(vec != NULL);
  struct _Vector* v = (struct _Vector*) vec;
  return v->length;
}

void VecClear(Vector* vec, VectorOp del, void* pt)
{
  struct _Vector* v = (struct _Vector*) vec;
  VecTruncate(vec, 0, del, pt);
}

void VecClear2(Vector* vec)
{
  VecClear(vec, NULL, NULL);
}

void VecTruncate(Vector* vec, int i, VectorOp del, void* pt)
{
}

void VecTruncate2(Vector* vec, int i)
{
  VecTruncate(vec, i, NULL, NULL);
}

void VecSetSize(Vector* vec, int size)
{
  assert(vec != NULL);
  struct _Vector* v = (struct _Vector*) vec;
  if (size >= v->length) {
  }
}

void VecAppend(Vector* vec, void* element)
{
}

void* VecGet(Vector* vec, int i)
{
  assert(vec != NULL);
  struct _Vector* v = (struct _Vector*) vec;
  int n = normalize_index(v, i);
  if (n >= 0) {
    void** p = get_slot(v, i);
    return *p;
  }
  
  return NULL;
}

void VecSet(Vector* vec, int i, void* element)
{
  assert(vec != NULL);
  struct _Vector* v = (struct _Vector*) vec;
  int n = normalize_index(v, i);
  if (n >= 0) {
    void** p = get_slot(v, i);
    *p = element;
  }  
}
