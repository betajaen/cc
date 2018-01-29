#include <stdlib.h>
#include <malloc.h>
#include <string.h>

int*  _dx8_calloc(nelem, elsize)
  int nelem;
  int elsize;
{
  int* mem;
  mem = malloc(nelem * elsize);
  memset(mem, 0, nelem * elsize);
  return mem;
}

_dx8_cfree(ptr)
  int* ptr;
{
  free(ptr);
}

_dx8_exit(status)
  int status;
{
  exit(status);
}

_dx8_zero(mem, memsize)
  int* mem;
  int memsize;
{
  memset(mem, 0, memsize);
}
