/*
  cc -- C Compiler for DX8A Computer
*/

#define TOKEN_NONE                0
#define TOKEN_PARENTHESES_OPEN    1
#define TOKEN_PARENTHESES_CLOSE   2
#define TOKEN_BRACE_OPEN          3
#define TOKEN_BRACE_CLOSE         4
#define TOKEN_HASH                5
#define TOKEN_SEMICOLON           6

struct Token
{
  int          token;
  char*        str;
  int          str_len;
};

struct Token* tokens;

tokfile(file)
char* file;
{
  
}
