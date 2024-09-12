#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "pvector.h"

#define I0(i) ( i & 0x000000FF)
#define I1(i) ((i & 0x0000FF00) >> 8)
#define I2(i) ((i & 0x00FF0000) >> 16)
#define I3(i) ((i & 0x7F000000) >> 24)

struct _PVector {
  int length;
  unsigned capacity; /* 1, 256, 256*256, 256*256*256, 128*256*256*256 */
  void* elements;
};

static int normalize_index(struct _PVector* v, int i)
{
    int n = i < 0 ? v->length + i : i;
    
    if (0 <= n && n < v->length) {
      return n;
    }
    else {
      return -1;
    }
}

static void* make_array(void)
{
  void* el = calloc(256, sizeof(void*));
  assert(el != NULL);
  //~ fprintf(stderr, "array = %p\n", el);
  return el;
}

static void free_array(void* arr)
{
  if (arr != NULL) {
    //~ fprintf(stderr, "array = %p X\n", arr);
    free(arr);
  }
}


static void grow(struct _PVector* v, unsigned size)
{
  while (v->capacity < size) {
    switch (v->capacity) {
    case 1:
      v->capacity = 256;
      break;
    case 256:
      v->capacity = 256*256;
      break;
    case 256*256:
      v->capacity = 256*256*256;
      break;
    case 256*256*256:
      v->capacity = (unsigned) 128*256*256*256;
      break;
    default:
      assert("Cannot grow array beyond 2,147,483,648 elements" == NULL);
    }
    void** el = make_array();
    el[0] = v->elements;
    v->elements = el;
  }
}

//~ switch (v->capacity) {
//~ case 1:
  //~ break;
  
//~ case 256:
  //~ break;
  
//~ case 256*256:
  //~ break;
  
//~ case 256*256*256:
  //~ break;
  
//~ case (unsigned) 128*256*256*256:
  //~ break;
  
//~ default:
//~ }

static void* vec_get(struct _PVector* v, int n)
{
  if (0 <= n && n < v->length) {
    void* p = NULL;
    void** el;
        
    switch (v->capacity) {
      
    case 1:
      return v->elements;
      
    case 256:
      el = (void**) v->elements;
      if (el == NULL) {
        goto exit;
      }
      return el[I0(n)];
      
    case 256*256:
      el = (void**) v->elements;
      if (el == NULL) {
        goto exit;
      }
      el = (void**) el[I1(n)];
      if (el == NULL) {
        goto exit;
      }
      return el[I0(n)];
      
    case 256*256*256:
      el = (void**) v->elements;
      if (el == NULL) {
        goto exit;
      }
      el = (void**) el[I2(n)];
      if (el == NULL) {
        goto exit;
      }
      el = (void*) el[I1(n)];
      if (el == NULL) {
        goto exit;
      }
      return el[I0(n)];
      
    case (unsigned) 128*256*256*256:
      el = (void**) v->elements;
      if (el == NULL) {
        goto exit;
      }
      el = (void**) el[I3(n)];
      if (el == NULL) {
        goto exit;
      }
      el = (void*) el[I2(n)];
      if (el == NULL) {
        goto exit;
      }
      el = el[I1(n)];
      if (el == NULL) {
        goto exit;
      }
      return el[I0(n)];
      
    default:
      assert("Illegal vector capacity" == NULL);
      return NULL;
    }
  }
  
exit:
  return NULL;
}

static void** vec_slot(struct _PVector* v, int n)
{
  if (0 <= n && n < v->length) {
    void** el;
    
    switch (v->capacity) {
      
    case 1:
      return &v->elements;
      
    case 256:
      el = &v->elements;
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      return &el[I0(n)];
      
    case 256*256:
      el = &v->elements;
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      el = &el[I1(n)];
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      return &el[I0(n)];
      
    case 256*256*256:
      el = &v->elements;
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      el = &el[I2(n)];
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      el = &el[I1(n)];
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      return &el[I0(n)];
      
    case (unsigned) 128*256*256*256:
      el = &v->elements;
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      el = &el[I3(n)];
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      el = &el[I2(n)];
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      el = &el[I1(n)];
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      return &el[I0(n)];
      
    default:
      assert("Illegal vector capacity" == NULL);
    }
  }
  
  return NULL;
}

PVector* PVecCreate(PVector* vec)
{
  assert(sizeof(PVector) >= sizeof(struct _PVector));
  struct _PVector* v = (struct _PVector*) vec;
  
  if (v == NULL) {
    v = malloc(sizeof(struct _PVector));
    assert(v != NULL);
  }
  
  v->length = 0;
  v->elements = NULL;
  v->capacity = 1;
  
  return (PVector*) v;
}

int PVecLength(PVector* vec)
{
  assert(vec != NULL);
  struct _PVector* v = (struct _PVector*) vec;
  return v->length;
}

void PVecClear(PVector* vec, VectorOp del, void* pt)
{
  struct _PVector* v = (struct _PVector*) vec;
  
  PVecTruncate(vec, 0, del, pt);
  
  switch (v->capacity) {
  case 256:
  {
    free_array(v->elements);
    break;
  }
    
  case 256*256:
  {
    void** el1 = v->elements;
    if (el1 != NULL) {
      int i;
      for (i = 0; i < 256; i++) {
        free_array(el1[i]);
      }
      free_array(el1);
    }
    break;
  }
    
  case 256*256*256:
  {
    void** el1 = v->elements;
    if (el1 != NULL) {
      int i;
      for (i = 0; i < 256; i++) {
        void** el2 = el1[i];
        if (el2 != NULL) {
          int j;
          for (j = 0; j < 256; j++) {
            free_array(el2[j]);
          }
          free_array(el2);
        }
      }
      free_array(el1);
    }
    break;
  }
    
  case (unsigned) 128*256*256*256:
  {
    void** el1 = v->elements;
    if (el1 != NULL) {
      int i;
      for (i = 0; i < 256; i++) {
        void** el2 = el1[i];
        if (el2 != NULL) {
          int j;
          for (j = 0; j < 256; j++) {
            void** el3 = el2[j];
            if (el3 != NULL) {
              int k;
              for (k = 0; k < 256; k++) {
                free_array(el3[k]);
              }
              free_array(el3);
            }
          }
          free_array(el2);
        }
      }
      free_array(el1);
    }
    break;
  }
    
  default:
  }
    
  v->capacity = 1;
  v->elements = NULL;
}

void PVecClear2(PVector* vec)
{
  PVecClear(vec, NULL, NULL);
}

void PVecTruncate(PVector* vec, int i, VectorOp del, void* pt)
{
  struct _PVector* v = (struct _PVector*) vec;
  assert(vec != NULL);
  int n = normalize_index(v, i);
  if (0 <= n && n < v->length) {
    if (del != NULL) {
      int k;
      for (k = n; k < v->length; k++) {
        del(k, vec_get(v, k), pt);
      }
    }
    v->length = n;
  }
}

void PVecTruncate2(PVector* vec, int i)
{
  PVecTruncate(vec, i, NULL, NULL);
}

void PVecAppend(PVector* vec, void* element)
{
  struct _PVector* v = (struct _PVector*) vec;
  
  assert(vec != NULL);
  int i = v->length;
  v->length++;
  grow(v, v->length);
  PVecSet(vec, i, element);
}

void** PVecAt(PVector* vec, int i)
{
  struct _PVector* v = (struct _PVector*) vec;
  
  assert(vec != NULL);
  assert(v->length <= v->capacity);
  
  int n = normalize_index(v, i);
  return vec_slot(v, n);
}

void* PVecGet(PVector* vec, int i)
{
  struct _PVector* v = (struct _PVector*) vec;
  
  assert(vec != NULL);
  assert(v->length <= v->capacity);
  
  int n = normalize_index(v, i);
  return vec_get(v, n);
}

void* PVecSet(PVector* vec, int i, void* element)
{
  void** p = PVecAt(vec, i);
  if (p != NULL) {
    void* previous = *p;
    *p = element;
    return previous;
  }
  else {
    return NULL;
  }
}
