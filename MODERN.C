/*
  cc -- C Compiler for DX8A Computer
*/

/* STDLIB.H */
#define calloc  _dx8_calloc
#define cfree   _dx8_cfree
#define exit    _dx8_exit

/* STDIO.H */
#define fopen   _dx8_fopen
#define fclose  _dx8_fclose
#define fread   _dx8_fread
#define fwrite  _dx8_fwrite
#define fseek   _dx8_fseek
#define printf  _dx8_printf
#define fprintf _dx8_fprintf

#define stdout  _dx8_stdout
#define stderr  _dx8_stderr

/* STRING.H */
#define strcmp _dx8_strcmp
#define strlen _dx8_strlen

/* Include 'CC.C' */
#include "CC.C"
