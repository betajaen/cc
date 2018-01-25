#include <stdio.h>

#define _DX8_MAX_FILES_OPEN 32
typedef int _DX8_FILE;

FILE* files[_DX8_MAX_FILES_OPEN];

_DX8_FILE* _dx8_stderr = 2;
_DX8_FILE* _dx8_stdout = 1;

FILE* _dx8_FileToFile(file)
  _DX8_FILE* file;
{
  int m = ((int) (file)) - 2;
  return(files[m]);
}

_DX8_FILE* _dx8_FileCreate(file)
  FILE* file;
{
  int i;
  i = 0;
  while(i < _DX8_MAX_FILES_OPEN)
  {
    if (files[i] == 0)
    {
      files[i] = file;
      return (_DX8_FILE*) (i + 2);
    }
    i++;
  }
}

_dx8_FileDestroy(file)
  _DX8_FILE* file;
{
  int m = ((int) (file)) - 2;
  files[m] = 0;
}

_DX8_FILE* _dx8_fopen(filename, type)
  char* filename;
  char* type;
{
  FILE* fp = fopen(filename, type);
  _DX8_FILE* f = _dx8_FileCreate(fp);
  return(f);
}

_dx8_fclose(file)
  _DX8_FILE* file;
{
  return fclose(_dx8_FileToFile(file));
}

_dx8_fread(ptr, size, nitems, stream)
  int* ptr;
  int  size;
  int  nitems;
  _DX8_FILE* stream;
{
  return fread(ptr, size, nitems, _dx8_FileToFile(stream));
}

_dx8_fwrite(ptr, size, nitems, stream)
  int* ptr;
  int  size;
  int  nitems;
  _DX8_FILE* stream;
{
  return fwrite(ptr, size, nitems, _dx8_FileToFile(stream));
}

//;  /* int* ptr, int size, int nitems, FILE *stream); */
_dx8_fseek(stream, offset, whence)
  _DX8_FILE* stream;
  int offset;
  int whence;
{
  return fseek(_dx8_FileToFile(stream), offset, whence);
}

_dx8_fprintf(file, format, x1, x2, x3, x4, x5, x6, x7, x8)
  _DX8_FILE* file;
  char* format;
{
  FILE* fp;

  if (file == _dx8_stderr)
    fp = stderr;
  else if (file == _dx8_stdout)
    fp = stdout;
  else
    fp = _dx8_FileToFile(file);

  fprintf(fp, format, x1, x2, x3, x4, x5, x6, x7, x8);
}

_dx8_printf(format, x1, x2, x3, x4, x5, x6, x7, x8)
  char* format;
{
  return _dx8_fprintf(_dx8_stdout, format, x1, x2, x3, x4, x5, x6, x7, x8);
}
