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
  if(TRUE)
  {
    asm("setq r0, 0");
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
  while(TRUE)
  {
    asm("WHILE ON");
  }
  vec->x = 0;
  vec->y = 0;
}
