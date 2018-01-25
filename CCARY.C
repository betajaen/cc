/*
  cc -- C Compiler for DX8A Computer
*/

struct Vector
{
  char* data;
  int   length;
  int   allocs;
  int   typesize;
};

extern int vecmake();
extern int vecquit();
extern int vecadd();
extern int vecexpand();

/* make vector */
vecmake(vec, allocs, typesize)
  struct Vector* vec;
  int allocs;
  int typesize;
{
  vec->typesize = typesize;
  vec->length   = 0;
  vec->allocs   = allocs;
  vec->data     = calloc(sizeof(char*), typesize);
}

/* free vector */
vecfree(vec)
  struct Vector* vec;
{
  cfree(vec->data);
  vec->allocs   = 0;
  vec->length   = 0;
  vec->typesize = 0;
  vec->data     = 0;
}

/* add to vector */
vecadd(vec, item)
  struct Vector* vec;
  int* item;
{
  vecexpand(vec, 1);
}

/* expand vector allocations */
vecexpand(vec, amount)
  struct Vector* vec;
  int amount;
{
  if (vec->length + amount >= vec->allocs)
  {
    cfree(vec->allocs);
  }
}
