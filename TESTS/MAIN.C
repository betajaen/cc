struct Vector
{
  char x;
  char y;
};

main()
{
  struct Vector vec;
  asm("set r0, 4");
}
