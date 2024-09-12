#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

#define I0(i) ( i & 0x000000FF)
#define I1(i) ((i & 0x0000FF00) >> 8)
#define I2(i) ((i & 0x00FF0000) >> 16)
#define I3(i) ((i & 0x7F000000) >> 24)

struct _InnerNode {
  void* children[256]; /* Pointer to _InnerNodes or leaf node, which is 256 * element size */
};

struct _Vector {
  int length; /* Number of elements in the vector */
  unsigned capacity; /* one of 0, 256, 256*256, 256*256*256, 128*256*256*256 */
  int element_size; /* Number of bytes stored per element in a leaf node */
  void* elements;
};

static int normalize_index(struct _Vector* v, int i)
{
    int n = i < 0 ? v->length + i : i;
    return 0 <= n && n < v->length ? n : -1;
}

static void* make_leaf_node(int size)
{
  void* el = calloc(256, size);
  assert(el != NULL);
  //~ fprintf(stderr, "array = %p\n", el);
  return el;
}

static struct _InnerNode* make_inner_node(void)
{
  struct _InnerNode* inner = malloc(sizeof(struct _InnerNode));
  assert(inner != NULL);
  memset(inner, 0, sizeof(struct _InnerNode));
}

//~ switch (v->capacity) {
//~ case 0:
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

static void grow(struct _Vector* v, unsigned size)
{
  if (v->capacity == 0) {
    v->capacity = 256;
    v->elements = make_leaf_node(v->element_size);
  }

  while (v->capacity < size) {
    switch (v->capacity) {
    case 256:
    case 256*256:
      v->capacity = v->capacity * 256;
      break;

    case 256*256*256:
      v->capacity = (unsigned) 128*256*256*256;
      break;

    default:
      assert("Cannot grow array beyond 2,147,483,648 elements" == NULL);
    }

    struct _InnerNode* inner = make_inner_node();
    inner->children[0] = v->elements;
    v->elements = inner;
  }
}

/* Returns the pointer to <size> bytes of the <n>-th element of array <leaf> */
static void* vec_leaf_element(void* leaf, int size, int n)
{
  assert(n < 256);
  void* p = leaf + n * size;
  return p;
}

/* Returns a pointer to the <element_size> bytes of element <n> */
static void* vec_slot(struct _Vector* v, int n)
{
  if (0 <= n && n < v->length) {
    assert(v->elements != NULL);

    struct _InnerNode** p_inner;
    struct _InnerNode* inner;
    void** p_leaf;


    switch (v->capacity) {

    case 0:
      return NULL;

    case 256:
      return vec_leaf_element(v->elements, v->element_size, I0(n));

    case 256*256:
      inner = v->elements;
      p_leaf = &inner->children[I1(n)];
      if (*p_leaf == NULL) {
        *p_leaf = make_leaf_node(v->element_size);
      }
      return vec_leaf_element(*p_leaf, v->element_size, I0(n));

    case 256*256*256:
      inner = v->elements;
      p_inner = (struct _InnerNode**) &inner->children[I2(n)];
      if (*p_inner == NULL) {
        *p_inner =  make_inner_node();
      }
      inner = *p_inner;
      p_leaf = &inner->children[I1(n)];
      if (*p_leaf == NULL) {
        *p_leaf = make_leaf_node(v->element_size);
      }
      return vec_leaf_element(*p_leaf, v->element_size, I0(n));

    case (unsigned) 128*256*256*256:
      inner = v->elements;
      p_inner = (struct _InnerNode**) &inner->children[I3(n)];
      if (*p_inner == NULL) {
        *p_inner =  make_inner_node();
      }
      inner = *p_inner;
      p_inner = (struct _InnerNode**) &inner->children[I2(n)];
      if (*p_inner == NULL) {
        *p_inner =  make_inner_node();
      }
      inner = *p_inner;
      p_leaf = &inner->children[I1(n)];
      if (*p_leaf == NULL) {
        *p_leaf = make_leaf_node(v->element_size);
      }
      return vec_leaf_element(*p_leaf, v->element_size, I0(n));

    default:
      assert("Illegal vector capacity" == NULL);
    }
  }

  return NULL;
}

Vector* VecCreate(Vector* vec, int size)
{
  assert(sizeof(Vector) >= sizeof(struct _Vector));
  assert(size > 0);
  struct _Vector* v = (struct _Vector*) vec;

  if (v == NULL) {
    v = malloc(sizeof(struct _Vector));
    assert(v != NULL);
  }

  v->length = 0;
  v->capacity = 0;
  v->element_size = size;
  v->elements = NULL;

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
    free(v->elements);
    break;

  case 256*256:
    {
      struct _InnerNode* inner1 = v->elements;
      if (inner1 != NULL) {
        int i1;
        for (i1 = 0; i1 < 256; i1++) {
          free(inner1->children[i1]);
        }
        free(inner1);
      }
    }
    break;

  case 256*256*256:
    {
      struct _InnerNode* inner1 = v->elements;
      if (inner1 != NULL) {
        int i1;
        for (i1 = 0; i1 < 256; i1++) {
          struct _InnerNode* inner2 = inner1->children[i1];
          if (inner2 != NULL) {
            int i2;
            for (i2 = 0; i2 < 256; i2++) {
              free(inner2->children[i2]);
            }
            free(inner2);
          }
        }
        free(inner1);
      }
    }
    break;

  case (unsigned) 128*256*256*256:
    {
      struct _InnerNode* inner1 = v->elements;
      if (inner1 != NULL) {
        int i1;
        for (i1 = 0; i1 < 256; i1++) {
          struct _InnerNode* inner2 = inner1->children[i1];
          if (inner2 != NULL) {
            int i2;
            for (i2 = 0; i2 < 256; i2++) {
              struct _InnerNode* inner3 = inner2->children[i2];
              if (inner3 != NULL) {
                int i3;
                for (i3 = 0; i3 < 256; i3++) {
                  free(inner3->children[i3]);
                }
                free(inner3);
              }
            }
            free(inner2);
          }
        }
        free(inner1);
      }
    }
    break;

    default:
  }

  v->capacity = 0;
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
        del(k, vec_slot(v, k), pt);
      }
    }
    v->length = n;
  }
}

void VecTruncate2(Vector* vec, int i)
{
  VecTruncate(vec, i, NULL, NULL);
}

void* VecAppend(Vector* vec, void* element)
{
  struct _Vector* v = (struct _Vector*) vec;

  assert(vec != NULL);
  int i = v->length;
  v->length++;
  grow(v, v->length);
  return VecSet(vec, i, element);
}

void* VecGet(Vector* vec, int i, void* element)
{
  struct _Vector* v = (struct _Vector*) vec;

  assert(vec != NULL);
  assert(v->length <= v->capacity);

  int n = normalize_index(v, i);
  if (n < 0) {
    return NULL;
  }

  void* el = vec_slot(v, n);
  if (el != NULL && element != NULL) {
    memcpy(element, el, v->element_size);
  }
  return el;
}

void* VecSet(Vector* vec, int i, void* element)
{
  struct _Vector* v = (struct _Vector*) vec;

  assert(vec != NULL);
  assert(v->length <= v->capacity);

  int n = normalize_index(v, i);
  if (n < 0) {
    return NULL;
  }

  void* el = vec_slot(v, n);
  if (el != NULL && element != NULL) {
    memcpy(el, element, v->element_size);
  }

  return el;
}
