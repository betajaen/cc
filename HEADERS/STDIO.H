typedef int FILE;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

extern FILE* stdout;
extern FILE* stderr;

extern FILE* fopen(filename, type);              /* char* filename, char* type */
extern int   fclose(file);                       /* FILE* file */
extern int   fread(ptr, size, nitems, stream);   /* int* ptr, int size, int nitems, FILE* stream); */
extern int   fwrite(ptr, size, nitems, stream);  /* int* ptr, int size, int nitems, FILE *stream); */
extern int   fseek(stream, offset, whence);      /* FILE* stream, int offset, int whence */
extern int   printf(format, x1, x2, x3, x4, x5, x6, x7, x8); /* char* format, ... */
extern int   fprintf(stream, format, x1, x2, x3, x4, x5, x6, x7, x8); /* FILE* file, char* format, ... */