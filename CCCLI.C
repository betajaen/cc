/*
  cc -- C Compiler for DX8A Computer
*/

main(argc, argv)
  int argc;
  char** argv;
{
  if (argc == 1)
  {
    exit(1);
  }

  if (tokopen(argv[1]) == FALSE)
  {
    printf("Cannot read file");
    exit(1);
  }

  nodinit();
  nodfile();
  tokclose();
  noddbg();
  asm();
  nodstop();
}

panic(text, x1, x2, x3, x4, x5, x6)
  char* text;
{
  printf("<Panic> ");
  printf(text, x1, x2, x3, x4, x5, x6);
  printf("\n");
  exit(1);
}
