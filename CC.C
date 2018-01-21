/*
  cc -- C Compiler for DX8A Computer
*/

/* Workaround for more modern compilers */

#include <stdio.h>
#include <stdlib.h>

cprint(c)
  char* c;
{
  fprintf(stdout, c);
}


#include "cccli.c"
#include "cctok.c"
#include "ccnod.c"
#include "ccasm.c"
