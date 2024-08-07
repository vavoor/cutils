#ifndef HEADER_a01d27a6_9f25_40a8_88a1_f2740f749d02
#define HEADER_a01d27a6_9f25_40a8_88a1_f2740f749d02

#include "plist.h"

typedef struct {
  PList symbols;
} SymTab;

SymTab* SymTabNew(SymTab* st);
void SymTabEnterScope(SymTab* st);
void SymTabLeaveScope(SymTab* st);
void* SymTabInsert(SymTab* st, const char* key, void* data);
void* SymTabFind(SymTab* st, const char* key);

#endif /* HEADER_a01d27a6_9f25_40a8_88a1_f2740f749d02 */
