/*
  cc -- C Compiler for DX8A Computer
*/

#define TOKEN_NONE                'X'
#define TOKEN_INTEGER             'i'
#define TOKEN_STRING              's'
#define TOKEN_NAME                'n'

#define TOKEN_PARENTHESES_OPEN    '('
#define TOKEN_PARENTHESES_CLOSE   ')'
#define TOKEN_BRACE_OPEN          '{'
#define TOKEN_BRACE_CLOSE         '}'
#define TOKEN_BRACKET_OPEN        '['
#define TOKEN_BRACKET_CLOSE       ']'
#define TOKEN_HASH                '#'
#define TOKEN_SEMICOLON           ';'
#define TOKEN_LESS                '<'
#define TOKEN_MORE                '>'
#define TOKEN_EQUALS              '='
#define TOKEN_ADD                 '+'
#define TOKEN_SUB                 '-'
#define TOKEN_MUL                 '*'
#define TOKEN_DIV                 '/'

#define STRING_MAX_SIZE           512
#define NAME_MAX_SIZE             32

FILE* tfile;
int   teof;
char  tch;
char  tstr[256];
int   tstrlen;
int   tint;

struct Token
{
  char tok;
  char ch;
  char* str;
  int   str_len;
};

int   tokisname(ch)
  register char ch;
{
  return(((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_'));
}

int  tokiswhitespace(ch)
  register char ch;
{
  return(ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
}

int tokisnumber(ch)
  register char ch;
{
  return(ch >= '0' && ch <= '9');
}

int tokisletter(ch)
  register char ch;
{
  return((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}

/* open token reader */
tokopen(file)
char* file;
{
  extern FILE* tfile;
  extern int   teof;
  extern char  tch;

  tfile = fopen(file, "rb");

  if (tfile == 0)
  {
    teof = TRUE;
    return(FALSE);
  }
  
  tch = 0;

  return(TRUE);
}

/* close token reader */
tokclose()
{
  extern FILE* tfile;
  if (tfile != 0)
  {
    fclose(tfile);
  }
}

/* read whitespace */
tokrwsp()
{
  extern int isspace();

  extern FILE* tfile;
  extern int   teof;
  extern char  tch;

  if (isspace(tch) == FALSE)
    return(tch);

  while(fread(&tch, 1, 1, tfile) == 1)
  {
    if (isspace(tch) == FALSE)
    {
      return(tch);
    }
  }

  teof = 1;
  return(FALSE);
}

/* read number */
tokreadi(tch)
char tch;
{
  extern FILE* tfile;
  extern char  tstr[256];

  tint = tch - '0';
  
  while(TRUE)
  {
    if (fread(&tch, 1, 1, tfile) == 0)
    {
      break;
    }

    if (tokisnumber(tch) == FALSE)
    {
      fseek(tfile, -1, SEEK_CUR);
      break;
    }

    tint *= 10;
    tch -= '0';
    tint += tch;
  }

  return(TOKEN_INTEGER);
}

/* read symbol */
tokreadn()
{
  extern FILE* tfile;
  extern char  tstr[256];
  extern char  tch;

  int i;

  tstr[0] = tch;
  
  i = 1;
  while(i < 32)
  {
    if (fread(&tch, 1, 1, tfile) == 0)
    {
      break;
    }

    if (tokisname(tch) == FALSE)
    {
      fseek(tfile, -1, SEEK_CUR);
      break;
    }

    tstr[i++] = tch;
  }

  tstr[i] = '\0';
  tstrlen = i;

  return(TOKEN_NAME);
}

/* read literal */
tokreadl()
{
  
  extern FILE* tfile;
  extern char  tstr[256];
  extern char  tch;

  tint = tch;
  
  while(TRUE)
  {
    if (fread(&tch, 1, 1, tfile) == 0)
    {
      break;
    }

    if (tch == '\'')
    {
      break;
    }

    tint *= 256;
    tint += tch;
  }

  return(TOKEN_INTEGER);
}

/* read string */
tokreads()
{
  extern FILE* tfile;
  extern char  tstr[256];
  extern char tch;
  
  int i;

  i = 0;
  while(i < 32)
  {
    if (fread(&tch, 1, 1, tfile) == 0)
    {
      break;
    }

    if (tch == '"')
    {
      break;
    }
    tstr[i++] = tch;
  }

  tstr[i] = '\0';
  tstrlen = i;

  return TOKEN_STRING;
}

/* reset token */
tokrst()
{
  extern char tstr[];
  extern int  tint;

  tstr[0] = '\0';
  tint    = 0;
}

/* read token */
tokread()
{
  extern int tokreadi();
  extern int tokreads();
  extern int tokreadl();
  extern int tokreads();

  extern int teof;
  extern char tch;

  if (fread(&tch, 1, 1, tfile) == FALSE)
  {
    teof = 1;
    return(TOKEN_NONE);
  }
  
  if (teof == 1)
  {
    return(TOKEN_NONE);
  }

  tch = tokrwsp();

  if (teof == 1)
  {
    return(TOKEN_NONE);
  }

  if (tch >= '0' && tch <= '9')
  {
    return(tokreadi(tch));
  }

  if (tokisname(tch) == TRUE)
  {
    return(tokreadn(tch));
  }

  if (tch == '\'')
  {
    return(tokreadl());
  }

  if (tch == '\"')
  {
    return(tokreads());
  }

  switch(tch)
  {
    case '(':
      return(TOKEN_PARENTHESES_OPEN);
    case ')':
      return(TOKEN_PARENTHESES_CLOSE);
    case '{':
      return(TOKEN_BRACE_OPEN);
    case '}':
      return(TOKEN_BRACE_CLOSE);
    case '[':
      return(TOKEN_BRACKET_OPEN);
    case ']':
      return(TOKEN_BRACKET_CLOSE);
    case ';':
      return(TOKEN_SEMICOLON);
    case '<':
      return(TOKEN_LESS);
    case '>':
      return(TOKEN_MORE);
    case '#':
      return(TOKEN_HASH);
    case '=':
      return(TOKEN_EQUALS);
    case '+':
      return(TOKEN_ADD);
    case '-':
      return(TOKEN_SUB);
    case '*':
      return(TOKEN_MUL);
    case '/':
      return(TOKEN_DIV);
  }

  return(TOKEN_NONE);
}

/* get token integer or literal */
tokgeti()
{
  extern int tint;
  return(tint);
}

/* get token name */
char* tokgetn()
{
  extern char tstr[256];
  return(tstr);
}

/* get token string */
char* tokgets()
{
  extern char tstr[256];
  return(tstr);
}

/* check token string */
tokchecks(s)
  char* s;
{
  extern char tstr[256];
  return strcmp(s, tstr) == 0;
}

/* copy and return token string */
char* tokcopys()
{
  char* str;
  int i;

  str = calloc(1, tstrlen + 1);
  i = 0;

  while(i < tstrlen)
  {
    str[i] = tstr[i];
    i++;
  }

  str[i] = '\0';

  return(str);
}

/* copy token string to given string, and set length */
tokcopys2(str)
  char* str;
{
  int i;
  i = 0;

  while(i < tstrlen)
  {
    str[i] = tstr[i];
    i++;
  }

  str[i] = '\0';

  return(str);
}
