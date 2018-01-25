/*
  cc -- C Compiler for DX8A Computer
*/

main(argc, argv)
  int argc;
  char** argv;
{
  extern int tokopen();
  extern int tokclose();
  extern int nodfile();
  extern int nodinit();
  extern int nodstop();

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
