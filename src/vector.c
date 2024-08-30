#include <assert.h>
#include <stdlib.h>

#include "vector.h"

#define I0(i) ( i & 0x000000FF)
#define I1(i) ((i & 0x0000FF00) >> 8)
#define I2(i) ((i & 0x00FF0000) >> 16)
#define I3(i) ((i & 0xFF000000) >> 24)

struct _Vector {
  int length;
  unsigned capacity; /* 1, 256, 256*256, 256*256*256, 128*256*256*256 */
  void* elements;
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

static void* make_array(void)
{
  void* el = calloc(256, sizeof(void*));
  assert(el != NULL);
  return el;
}


static void grow(struct _Vector* v, unsigned size)
{
  while (size > v->capacity) {
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

static void* vec_get(struct _Vector* v, int n)
{
  //~ if (n == 256) {
    //~ int x = 17;
  //~ }
  if (0 <= n && n < v->length) {
    void* p = NULL;
    void** el;
    
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
    
    switch (v->capacity) {
      
    case 1:
      p = v->elements;
      break;
      
    case 256:
      el = (void**) v->elements;
      if (el != NULL) {
        p = el[I0(n)];
      }
      break;
      
    case 256*256:
      el = (void**) v->elements;
      if (el != NULL) {
        el = (void**) el[I1(n)];
        if (el != NULL) {
          p = el[I0(n)];
        }
      }
      break;
      
    case 256*256*256:
      el = (void**) v->elements;
      if (el != NULL) {
        el = (void**) el[I2(n)];
        if (el != NULL) {
          el = (void*) el[I1(n)];
          if (el != NULL) {
            p = el[I0(n)];
          }
        }
      }
      break;
      
    case (unsigned) 128*256*256*256:
      el = (void**) v->elements;
      if (el != NULL) {
        el = (void**) el[I3(n)];
        if (el != NULL) {
          el = (void*) el[I2(n)];
          if (el != NULL) {
            el = el[I1(n)];
            if (el != NULL) {
              p = el[I0(n)];
            }
          }
        }
      }
      break;
      
    default:
      assert("Illegal vector capacity" == NULL);
    }
    
    return p;
  }
  
  return NULL;
}

static void* vec_set(struct _Vector* v, int n, void* element)
{
  if (0 <= n && n < v->length) {
    void* p = NULL;
    void** el;
    
    switch (v->capacity) {
      
    case 1:
      p = v->elements;
      v->elements = element;
      break;
      
    case 256:
      el = &v->elements;
      if (*el == NULL) {
        *el = make_array();
      }
      el = *el;
      p = el[I0(n)];
      el[I0(n)] = element;
      break;
      
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
      p = el[I0(n)];
      el[I0(n)] = element;
      break;
      
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
      p = el[I0(n)];
      el[I0(n)] = element;
      break;
      
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
      p = el[I0(n)];
      el[I0(n)] = element;
      break;
      
    default:
      assert("Illegal vector capacity" == NULL);
    }
    return p;
  }
  
  return NULL;
}

Vector* VecCreate(Vector* vec)
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

int VecLength(Vector* vec)
{
  assert(vec != NULL);
  struct _Vector* v = (struct _Vector*) vec;
  return v->length;
}

void VecClear(Vector* vec, VectorOp del, void* pt)
{
  struct _Vector* v = (struct _Vector*) vec;
  VecTruncate(vec, 0, del, pt);
  
  switch (v->capacity) {
  case 256:
  {
    free(v->elements);
    break;
  }
    
  case 256*256:
  {
    void** el1 = v->elements;
    if (el1 != NULL) {
      int i;
      for (i = 0; i < 256; i++) {
        free(el1[i]);
      }
      free(el1);
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
            free(el2[j]);
          }
          free(el2);
        }
      }
      free(el1);
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
                free(el3[k]);
              }
              free(el3);
            }
          }
          free(el2);
        }
      }
      free(el1);
    }
    break;
  }
    
  default:
  }
    
  v->capacity = 1;
  v->elements = NULL;
}

void VecClear2(Vector* vec)
{
  VecClear(vec, NULL, NULL);
}

void VecTruncate(Vector* vec, int i, VectorOp del, void* pt)
{
  struct _Vector* v = (struct _Vector*) vec;
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

void VecTruncate2(Vector* vec, int i)
{
  VecTruncate(vec, i, NULL, NULL);
}

void VecAppend(Vector* vec, void* element)
{
  struct _Vector* v = (struct _Vector*) vec;
  
  assert(vec != NULL);
  int i = v->length;
  v->length++;
  grow(v, v->length);
  VecSet(vec, i, element);
}

void* VecGet(Vector* vec, int i)
{
  struct _Vector* v = (struct _Vector*) vec;
  
  assert(vec != NULL);
  assert(v->length <= v->capacity);
  
  int n = normalize_index(v, i);
  return vec_get(v, n);
}

void* VecSet(Vector* vec, int i, void* element)
{
  struct _Vector* v = (struct _Vector*) vec;
  
  assert(vec != NULL);
  assert(v->length <= v->capacity);
  
  int n = normalize_index(v, i);
  return vec_set(v, n, element);
}
