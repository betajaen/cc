struct Vector
{
  char x;
  char y;
};

union Vector2
{
  struct Vector vec;
  int mmm;
};

main()
{
  struct Vector vec;
  asm("set r0, 4");
}
