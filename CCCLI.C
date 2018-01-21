/*
  cc -- C Compiler for DX8A Computer
*/

#include <stdio.h>

tokfile(file); /* CCTOK.C */

main(argc, argv)
  int argc;
  char** argv;
{
  if (argc == 1)
  {
    cprint("Missing C file");
    exit(1);
  }

  tokfile(argv[1]);
}
