#include <assert.h>
#include <stdlib.h>

#include "sym.h"
#include "pmap.h"

SymTab* SymTabNew(SymTab* st)
{
  if (st == NULL) {
    st = malloc(sizeof(SymTab));
    assert(st != NULL);
  }
  PListNew(&st->symbols);
  return st;
}

void SymTabEnterScope(SymTab* st)
{
  assert(st != NULL);
  PMap* m = PMapNew(NULL);
  PListPush(&st->symbols, m);
}

void SymTabLeaveScope(SymTab* st)
{
  assert(st != NULL);
  PMap* m = PListPop(&st->symbols);
  if (m != NULL) {
    PMapClear(m, NULL);
  }
}

void* SymTabInsert(SymTab* st, const char* key, void* data)
{
  assert(st != NULL);
  assert(key != NULL);
  assert(data != NULL);

  PMap* m = PListTop(&st->symbols);
  assert(m != NULL);
  return PMapOverwrite(m, key, data);
}

void* SymTabFind(SymTab* st, const char* key)
{
  assert(st != NULL);
  assert(key != NULL);

  void* it;
  PListFirst(&st->symbols, &it);
  while (it != NULL) {
    PMap* m = PListNext(&it);
    void* data = PMapGet(m, key);
    if (data != NULL) {
      return data;
    }
  }
  return NULL;
}
