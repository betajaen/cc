#include <string.h>

int _dx8_strcmp(s1, s2)
  char* s1;
  char* s2;
{
  return strcmp(s1, s2);
}

int _dx8_strlen(s)
  char* s;
{
  return strlen(s);
}

int _dx8_isalpha(c)
  char c;
{
  return isalpha(c);
}

int _dx8_islower(c)
  char c;
{
  return islower(c);
}

int _dx8_isspace(c)
  char c;
{
  return isspace(c);
}

int _dx8_toupper(c)
  char c;
{
  return toupper(c);
}

int _dx8_tolower(c)
  char c;
{
  return tolower(c);
}
