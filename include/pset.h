#ifndef HEADER_9127986e_2a80_4806_b7e1_ef2ce43fc5ef
#define HEADER_9127986e_2a80_4806_b7e1_ef2ce43fc5ef

typedef struct _PSet {
  int count;
  int capacity;
  void** elements;
} PSet;

PSet* PSetNew(PSet* set);
void PSetClear(PSet* set);
int PSetSize(PSet* set);

/* Adds element to set. Returns true if element has already been present in the set. */
int PSetAdd(PSet* set, void* element);
int PSetContains(PSet* set, void* element);

void PSetUnion(PSet* a, PSet* b, PSet* u);
void PSetIntersect(PSet* a, PSet* b, PSet* u);


#endif /* HEADER_9127986e_2a80_4806_b7e1_ef2ce43fc5ef */
