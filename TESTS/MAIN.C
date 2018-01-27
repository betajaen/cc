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
  if(FALSE)
  {
    asm("setq r0, 1");
  }
  else
  {
    asm("setq r0, 1");
  }
}

add(a, b)
  int a;
  int b;
{
  a += b;
  asm("setq r0, r1");
}

clear(vec)
  struct Vector* vec;
{
  vec->x = 0;
  vec->y = 0;
}
