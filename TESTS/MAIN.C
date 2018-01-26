struct Vector
{
  char x;
  char y;
};

print_vector(vec)
  struct Vector* vec;
{
  vec = 2;
  vec->x = 5;
  vec[3] = 7;
  vec[vec->y[vec->x]] = 2;
}
