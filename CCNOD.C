/*
  cc -- C Compiler for DX8A Computer
*/

#define NODE_TYPE_NONE            0
#define NODE_TYPE_FILE            1
#define NODE_TYPE_EOF             2
#define NODE_TYPE_SYMBOL          3
#define NODE_TYPE_FUNCTION        4
#define NODE_TYPE_SCOPE           5
#define NODE_TYPE_NUMBER          6
#define NODE_TYPE_ASSEMBLY        7
#define NODE_TYPE_WHILE           8
#define NODE_TYPE_RETURN          9
#define NODE_TYPE_BREAK           10
#define NODE_TYPE_STRING          11
#define NODE_TYPE_ASNOP           12

#define SCOPE_TYPE_FUNCTION       0
#define SCOPE_TYPE_IF             1
#define SCOPE_TYPE_WHILE          2

#define ASNOP_COPY                0 /* a =  b; */
#define ASNOP_ADD                 1 /* a += b; */
#define ASNOP_SUB                 2 /* a -= b; */
#define ASNOP_MUL                 3 /* a *= b; */
#define ASNOP_DIV                 4 /* a /= b; */
#define ASNOP_MOD                 5 /* a %= b; */
#define ASNOP_XOR                 6 /* a ^= b; */
#define ASNOP_OR                  7 /* a |= b; */
#define ASNOP_AND                 8 /* a &= b; */
#define ASNOP_INC                 9 /* a ++;   */
#define ASNOP_DEC                10 /* a --;   */
#define ASNOP_SHL                11 /* a <<=   */
#define ASNOP_SHR                12 /* a >>=   */

#define KEYWORD_TYPE_INT          0
#define KEYWORD_TYPE_CHAR         1
#define KEYWORD_TYPE_SIZE         2

#define CONSTANT_FALSE     0
#define CONSTANT_TRUE      1
#define CONSTANT_NO        2
#define CONSTANT_YES       3
#define CONSTANT_SIZE      4

extern panic();
extern read();
extern expect();
extern expect_name();
extern read_and_expect();
extern dont_expect();
extern read_and_dont_expect();
extern char* tokgetn();
extern int tokread();
extern char* tokcopys();
extern tokcopy2();
extern int tokchecks();

char  nstra[256];
char  nstrb[256];

struct Keyword
{
  char* text;
  int   id;
};

struct Keyword keywords[KEYWORD_TYPE_SIZE];

struct Constant
{
  char* text;
  int   id;
  int   val;
};

struct Constant constants[CONSTANT_SIZE];

struct Node
{
  int          type;
  int          sub_type;
  int          symbol;
  int          index;
  char*        text;
  struct Node* next;
  struct Node* prev;
  struct Node* first;
  struct Node* last;
  int          integer;
};

int nodeidx;
struct Node* nodes;
int token;

struct Node* nodmk(type, parent)
  int type;
  struct Node* parent;
{
  extern int nodeidx;
  extern nodadd();

  struct Node* node;
  node = calloc(1, sizeof(struct Node));
  node->type = type;
  node->sub_type = 0;
  node->symbol = 0;
  node->index = nodeidx++;
  node->text = 0;
  node->next = 0;
  node->prev = 0;
  node->first = 0;
  node->last = 0;
  node->integer = 0;

  if (parent != 0)
  {
    if (parent->first == 0)
    {
      parent->first = node;
      parent->last  = node;
      node->prev = 0;
      node->next = 0;
    }
    else
    {
      parent->last->next = node;
      node->prev = parent->last;
      node->next = 0;
      parent->last = node;
    }
  }

  return(node);
}

nodprint(node, depth)
  struct Node* node;
  int depth;
{
  int i;
  struct Node* child;

  if (depth == 8)
    return;
  
  i = 0;
  while(i < depth)
  {
    printf("-");
    i++;
  }

  switch(node->type)
  {
    case NODE_TYPE_NONE:
      printf("+ none");
    break;
    case NODE_TYPE_FILE:
      printf("+ file");
    break;
    case NODE_TYPE_EOF:
      printf("+ EOF");
    break;
    case NODE_TYPE_SYMBOL:
      printf("+ %s", node->text);
    break;
    case NODE_TYPE_FUNCTION:
      printf("+ %s()", node->text);
    break;
    case NODE_TYPE_SCOPE:
      printf("+ {}");
    break;
    case NODE_TYPE_NUMBER:
      printf("+ %i", node->integer);
    break;
    case NODE_TYPE_ASSEMBLY:
      printf("+ ASM %s", node->text);
    break;
    case NODE_TYPE_WHILE:
      printf("+ while");
    break;
    case NODE_TYPE_RETURN:
      printf("+ return");
    break;
    case NODE_TYPE_BREAK:
      printf("+ break");
    break;
    case NODE_TYPE_STRING:
      printf("+ '%s'", node->text);
    break;
    case NODE_TYPE_ASNOP:
      printf("+ ASNOP ");
      switch(node->sub_type)
      {
        case ASNOP_COPY: printf("=") ; break;
        case ASNOP_ADD:  printf("+="); break;
        case ASNOP_SUB:  printf("-="); break;
        case ASNOP_MUL:  printf("*="); break;
        case ASNOP_DIV:  printf("/="); break;
        case ASNOP_MOD:  printf("%="); break;
        case ASNOP_XOR:  printf("^="); break;
        case ASNOP_OR:   printf("|="); break;
        case ASNOP_AND:  printf("&="); break;
        case ASNOP_INC:  printf("++"); break;
        case ASNOP_DEC:  printf("--"); break;
        case ASNOP_SHL:  printf("<<="); break;
        case ASNOP_SHR:  printf(">>="); break;
      }
    break;
  }

  printf("\n");

  child = node->first;
  while(child != 0)
  {
    nodprint(child, depth + 1);
    child = child->next;
  }

}

nodtoktokw()
{
  char* n0;
  int   i = 0;

  n0 = tokgetn();
  
  while(i < KEYWORD_TYPE_SIZE)
  {
    char* n1;
    
    n1 = keywords[i].text;

    if (strcmp(n0, n1) == 0)
    {
      return i;
    }

    i++;
  }

  return(KEYWORD_TYPE_SIZE);
}

nodtoconst()
{
  char* n0;
  int   i = 0;

  n0 = tokgetn();
  
  while(i < CONSTANT_SIZE)
  {
    char* n1;
    
    n1 = constants[i].text;

    if (strcmp(n0, n1) == 0)
    {
      return i;
    }

    i++;
  }

  return(CONSTANT_SIZE);
}

nodkw(name, id)
  char* name;
  int id;
{
  extern struct Keyword keywords[];
  struct Keyword keyword;
  keyword.text = name;
  keyword.id   = id;

  keywords[id] = keyword;
}

nodconst(name, id, val)
  char* name;
  int id;
  int val;
{
  extern struct Constant constants[];
  struct Constant constant;
  constant.text = name;
  constant.id   = id;
  constant.val  = val;

  constants[id] = constant;
}

nodrdasm(parent)
  struct Node* parent;
{
  struct Node* node;

  expect_name("asm");
  read_and_expect('(');
  read_and_expect('s');
  
  node = nodmk(NODE_TYPE_ASSEMBLY, parent);
  node->text = tokcopys();
  
  read_and_expect(')');
  read_and_expect(';');
}

nodrdasnopr(parent)
  struct Node* parent;
{
  struct Node* right;
  read();

  if (token == 'i')
  {
    right = nodmk(NODE_TYPE_NUMBER, parent);
    right->integer = tokgeti();
  }
  else if (token == 'n')
  {
    right = nodmk(NODE_TYPE_SYMBOL, parent);
    right->text = tokcopys();
  }
  else
  {
    panic("Unknown right token type");
  }
}

/*
   a =  b;
   a += b;
   a -= b;
   a *= b;
   a /= b;
   a %= b;
   a ^= b;
   a |= b;
   a &= b;
   a ++;
   a --;
   a <<= b;
   a >>= b;
*/
nodrdasnop(parent)
  struct Node* parent;
{
  struct Node* node;
  struct Node* left;
  
  node = nodmk(NODE_TYPE_ASNOP, parent);
  left = nodmk(NODE_TYPE_SYMBOL, node);
  left->text = tokcopys();

  read();
  
  switch(token)
  {
    case '=':
    {
      node->sub_type = ASNOP_COPY;
      nodrdasnopr(node);
    }
    break;
    case '+':
    {
      read();
      if (token == '+')
      {
        node->sub_type = ASNOP_INC;
      }
      else if (token == '=')
      {
        node->sub_type = ASNOP_ADD;
        nodrdasnopr(node);
      }
      else
      {
        panic("Unknown +? token");
      }
    }
    break;
    case '-':
    {
      read();
      if (token == '-')
      {
        node->sub_type = ASNOP_DEC;
      }
      else if (token == '=')
      {
        node->sub_type = ASNOP_SUB;
        nodrdasnopr(node);
      }
      else
      {
        panic("Unknown -? token");
      }
    }
    break;
    case '/':
    {
      read_and_expect('=');
      node->sub_type = ASNOP_DIV;
      nodrdasnopr(node);
    }
    break;
    case '*':
    {
      read_and_expect('=');
      node->sub_type = ASNOP_MUL;
      nodrdasnopr(node);
    }
    break;
    case '%':
    {
      read_and_expect('=');
      node->sub_type = ASNOP_MOD;
      nodrdasnopr(node);
    }
    break;
    case '^':
    {
      read_and_expect('=');
      node->sub_type = ASNOP_XOR;
      nodrdasnopr(node);
    }
    break;
    case '|':
    {
      read_and_expect('|');
      node->sub_type = ASNOP_XOR;
      nodrdasnopr(node);
    }
    break;
    case '&':
    {
      read_and_expect('&');
      node->sub_type = ASNOP_AND;
      nodrdasnopr(node);
    }
    break;
    case '<':
    {
      read_and_expect('<');
      read_and_expect('=');
      node->sub_type = ASNOP_SHL;
      nodrdasnopr(node);
    }
    break;
    case '>':
    {
      read_and_expect('>');
      read_and_expect('=');
      node->sub_type = ASNOP_SHR;
      nodrdasnopr(node);
    }
    break;
    default:
      printf("token=%c\n", token);
      panic("Unknown asnop symbol");
    break;
  }

  read_and_expect(';');

}

nodrdscope(parent)
  struct Node* parent;
{
  extern int   token;
  struct Node* scope;
  
  scope = nodmk(NODE_TYPE_SCOPE, parent);
  expect('{');
  while(read() != 'X')
  {
    printf("%c", token);

    dont_expect('{');

    if (token == '}')
      break;
    
    /*
      asm("assembly");
      asnop
    */
    if (token == 'n')
    {
      printf("%s\n", tokgets());
      if (tokchecks("asm"))
      {
        nodrdasm(scope);
      }
      else
      {
        printf("asnop=%c\n", token);
        nodrdasnop(scope);
      }
      continue;
    }
  }
  printf("\n");
}


nodrdfun()
{
  /*
    In the form of
      main()
      {
        Scope
      }
  */
  struct Node* node = nodmk(NODE_TYPE_FUNCTION, nodes);
  node->text = tokcopys();
  
  read_and_expect('(');
  read_and_expect(')');
  read_and_expect('{');

  nodrdscope(node);
}

/* read declaration from global scope*/
nodrdglobdecl()
{
  int kw;
  kw = nodtoktokw();

  if (kw != KEYWORD_TYPE_SIZE)
  {
    panic("Unexpected keyword");
  }

  // If it's not a keyword, then it's probably a function.
  nodrdfun();
}

nodinit()
{
  nodkw("int", KEYWORD_TYPE_INT);
  nodkw("char", KEYWORD_TYPE_CHAR);

  nodconst("FALSE", CONSTANT_FALSE, 0);
  nodconst("TRUE", CONSTANT_TRUE, 1);
  nodconst("NO", CONSTANT_NO, 0);
  nodconst("YES", CONSTANT_YES, 1);

  nodes = nodmk(NODE_TYPE_FILE, 0);
}

nodstop()
{
  cfree(nodes);
}

nodfile()
{
  while(TRUE)
  {
    int token;
    
    token = read();

    if (token == 'X')
      break;

    // name
    // main() {}
    if (token == 'n')
    {
      nodrdglobdecl();
      continue;
    }
  }

  nodprint(nodes, 0);
}

expect(ch)
  char ch;
{
  extern int token;
  
  if (token == 'X')
  {
    panic("Unxpected End of File");
  }

  if (token != ch)
  {
    panic("Unexpected token. Want a %c. Got %c", ch, token);
  }
}

expect_name(name)
  char* name;
{
  if (token != 'n')
  {
    panic("Not name token");
  }

  return tokchecks(name);
}

read()
{
  extern int token;
  token = tokread();
  return token;
}

read_and_expect(ch)
  char ch;
{
  read();
  expect(ch);
}

dont_expect(ch)
  char ch;
{
  if (token == ch)
  {
    panic("Unwanted token. Got %c", ch);
  }
}

read_and_dont_expect(ch)
  char ch;
{
  read();
  dont_expect(ch);
}

#if NO
    switch(token)
    {
      case 'i':
        node = nodmk(token);

        printf("Integer = %i\n", tokgeti());
      break;
      case 's':
        printf("String = %s\n", tokgets());
      break;
      case 'n':
        printf("Name = %s\n", tokgetn());
      break;
      case '(':
        printf("Syntax = (\n");
      break;
      case ')':
        printf("Syntax = )\n");
      break;
      case '{':
        printf("Syntax = {\n");
      break;
      case '}':
        printf("Syntax = }\n");
      break;
      case ';':
        printf("Syntax = ;\n");
      break;
      case '#':
        printf("Syntax = #\n");
      break;
      default:
        printf("Unknown %c\n", token);
      break;
    }
#endif