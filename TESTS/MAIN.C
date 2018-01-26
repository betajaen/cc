struct Vector
{
  char x;
  char y;
};

union Combined
{
  struct Vector m;
  int z;
};

print_vector(vec)
  struct Vector* vec;
{
  asm("test");
}
