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

#define SCOPE_TYPE_FUNCTION       0
#define SCOPE_TYPE_IF             1
#define SCOPE_TYPE_WHILE          2

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
extern read_and_expect();
extern char* tokgetn();
extern char* strcmp();
extern int tokread();
extern char* tokcopys();

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
      printf("+ ASM %i", node->text);
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

nodrdscope(parent)
  struct Node* parent;
{
  extern int   token;
  struct Node* node;
  
  node = nodmk(NODE_TYPE_SCOPE, parent);
  expect('{');
  while(read() != 'X')
  {
    if (token == '}')
      break;
  }
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
{ extern int token;
  if (token == 'X')
  {
    panic("Unxpected End of File");
  }

  if (token != ch)
  {
    panic("Unexpected token. Want a %c. Got %c", ch, token);
  }
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