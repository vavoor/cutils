#include <assert.h>
#include <stdlib.h>

#include "vector.h"

struct _Vector {
  int length;
  long capacity;
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
  assert(0 <= i && i < v->capacity);
  
  if (v->capacity == 1) {
    return &v->elements;
  }
  else if (v->capacity == 256) {
    int i0 = i  & 0x000000FF;
    void** p = &v->elements;
    return &p[i0];
  }
  else if (v->capacity == 256*256) {
    int i0 = i & 0x000000FF;
    int i1 = (i & 0x0000FF00) >> 8;
    void** p = v->elements;
    p = p[i1];
    return &p[i0];
  }
  else if (v->capacity == 256*256*256) {
    int i0 = i & 0x000000FF;
    int i1 = (i & 0x0000FF00) >> 8;
    int i2 = (i & 0x00FF0000) >> 16;
    void** p = v->elements;
    p = p[i2];
    p = p[i1];
    return &p[i0];
  }
  else if (v->capacity == 256*256*256*256) {
    int i0 = (i & 0x000000FF);
    int i1 = (i & 0x0000FF00) >> 8;
    int i2 = (i & 0x00FF0000) >> 16;
    int i3 = (i & 0xFF000000) >> 24;
    void** p = v->elements;
    p = p[i3];
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
  v->capacity = 1;
  
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

static void increase_depth(struct _Vector* v)
{
  void** e = calloc(sizeof(void*), 256);
  assert(e != NULL);
  e[0] = v->elements;
  v->elements = e;
}

void VecAppend(Vector* vec, void* element)
{
}

void* VecGet(Vector* vec, int i)
{
  assert(vec != NULL);
  struct _Vector* v = (struct _Vector*) vec;
  int n = normalize_index(v, i);
  if (0 <= n && n < v->length) {
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

static void** fun(int depth, void** elements, int idx)
{
  void** e = element;
  int level;
  int i[4];
  i[0] = idx & 0x00FF;
  i[1] = (idx >> 8) & 0x00FF;
  i[2] = (idx >> 16) & 0x00FF;
  i[3] = (idx >> 24) & 0x00FF;
  
  for (level = 0; level < depth; level++) {
    if (*e == NULL) {
      *e = calloc(sizeof(void*), 256);
      assert(*e != NULL);
    }
    *e = &e[i];
  }
}
