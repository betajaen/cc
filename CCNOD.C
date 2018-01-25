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
#define NODE_TYPE_TYPEDECL        13
#define NODE_TYPE_CTYPE           14
#define NODE_TYPE_CTYPES          15
#define NODE_TYPE_SYMBOLS         16
#define NODE_TYPE_STRUCT          17

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

#define NODE_FLAGS_FUNCTION_FLAGS_DECLARED    1
#define NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL    2
#define NODE_FLAGS_FUNCTION_FLAGS_SCOPE        4
#define NODE_FLAGS_FUNCTION_FLAGS_ARGS         8
#define NODE_FLAGS_TYPE_FLAGS_STRUCT           16
#define NODE_FLAGS_TYPEDECL_FLAGS_EXTERN       32
#define NODE_FLAGS_TYPEDECL_FLAGS_POINTER      64
#define NODE_FLAGS_TYPEDECL_FLAGS_INTEGERISIZE 128
#define NODE_FLAGS_TYPEDECL_FLAGS_FUNCTION     256

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
  struct Node* ctype;
  struct Node* cond;
  int          integer;
  int          flags;
};

int nodeidx;
struct Node* cexterns;
struct Node* ctypes;
struct Node* csymbols;

int token;

struct Node* nodmk(type, parent)
  int type;
  struct Node* parent;
{
  extern int nodeidx;
  extern nodadd();

  struct Node* node;
  node = calloc(1, sizeof(struct Node));
  node->type      = type;
  node->sub_type  = 0;
  node->symbol    = 0;
  node->index     = nodeidx++;
  node->text      = 0;
  node->next      = 0;
  node->prev      = 0;
  node->first     = 0;
  node->last      = 0;
  node->ctype     = 0;
  node->integer   = 0;
  node->cond      = 0;
  node->flags     = 0;

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

nodmkctype(name, size)
 char* name;
 int size;
{
  struct Node* node;
  node = nodmk(NODE_TYPE_CTYPE, ctypes);
  node->text = name;
  node->integer = size;
}


nodfindctype(name)
 char* name;
{
  struct Node* ctype;
  ctype = ctypes->first;

  while(ctype != 0)
  {
    if (strcmp(ctype->text, name) == 0)
    {
      return ctype;
    }
    ctype = ctype->next;
  }

  return 0;
}

nodfindcsym(name)
  char* name;
{
  struct Node* csym;
  csym = csymbols->first;

  while(csym != 0)
  {
    if (strcmp(csym->text, name) == 0)
    {
      return csym;
    }
    csym = csym->next;
  }

  return 0;
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
      printf("+ File");
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
    case NODE_TYPE_CTYPE:
      printf("+ Type '%s', Size=%i", node->text, node->integer);
    break;
    case NODE_TYPE_CTYPES:
      printf("+ CTypes");
    break;
    case NODE_TYPE_TYPEDECL:
      printf("+ Type Decl '%s'", node->text);
    break;
    case NODE_TYPE_SYMBOLS:
      printf("+ CSymbols");
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
  
  if (node->ctype != 0)
  {
    nodprint(node->ctype, depth + 1);
  }
  
  if (node->cond != 0)
  {
    nodprint(node->cond, depth + 1);
  }

  if ((node->flags & NODE_FLAGS_TYPEDECL_FLAGS_POINTER) != 0)
  {
    i = 0;
    while(i < depth)
    {
      printf("-");
      i++;
    }
    printf("+ Is Pointer: TRUE\n");
  }
  
  if ((node->flags & NODE_FLAGS_TYPEDECL_FLAGS_EXTERN) != 0)
  {
    i = 0;
    while(i < depth)
    {
      printf("-");
      i++;
    }
    printf("+ Is Extern: TRUE\n");
  }

  if ((node->flags & NODE_FLAGS_TYPEDECL_FLAGS_INTEGERISIZE) != 0)
  {
    i = 0;
    while(i < depth)
    {
      printf("-");
      i++;
    }
    printf("+ Array Size: %i\n", node->integer);
  }
  
  if ((node->flags & NODE_FLAGS_TYPEDECL_FLAGS_FUNCTION) != 0)
  {
    i = 0;
    while(i < depth)
    {
      printf("-");
      i++;
    }
    printf("+ Is Function Decl: TRUE\n");
  }

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

/* node keyword */
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

/* node integer symbol */
nodsymi(name, id, val)
  char* name;
  int id;
  int val;
{
  struct Node* node;
  node = nodmk(NODE_TYPE_SYMBOL, csymbols);
  node->text = name;
  
  struct Node* value;
  value = nodmk(NODE_TYPE_NUMBER, node);
  value->integer = val;
  value->ctype   = nodfindctype("int");
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

nodrdwhile(parent)
  struct Node* parent;
{
  struct Node* while_;
  struct Scope* scope;

  while_ = nodmk(NODE_TYPE_WHILE, parent);
  
  read_and_expect('(');
  read();

  /* while(TRUE)
     while(FALSE)
     while(any_symbol)
  */
  if (token == 'n')
  {
    while_->cond = nodfindcsym(tokgets());
  }
  /*
    while(0)  -> while(FALSE)
    while(1)  -> while(TRUE)
    while(any other number) -> while(TRUE)
  */
  else if (token == 'i')
  {
    if (tokgeti() == 0)
    {
      while_->cond = nodfindcsym("FALSE");
    }
    else
    {
      while_->cond = nodfindcsym("TRUE");
    }
  }
  else
  {
    panic("Unexpected symbol for while");
  }
  
  read_and_expect(')');
  read_and_expect('{');
  nodrdscope(while_);
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

/* nod read typedecl */
nodrdtypedecl(scope)
  struct Node* scope;
{
  int is_struct;
  int is_pointer;
  int is_extern;
  int is_function;
  struct Node* node;

  is_struct  = FALSE;
  is_pointer = FALSE;
  is_extern  = FALSE;

  node       = nodmk(NODE_TYPE_TYPEDECL, scope);

  /*
    int  x;
    int* x;
    int  x[];
    int  x[4];
    struct test x;
    struct test* x;
    struct test  x[];
    struct test  y[];
    extern int   fn();
  */
  char* type_name = tokgets();

  if (strcmp(type_name, "extern") == 0)
  {
    is_extern = TRUE;
    node->flags |= NODE_FLAGS_TYPEDECL_FLAGS_EXTERN;
    read();
  }

  if (strcmp(type_name, "struct") == 0)
  {
    is_struct = TRUE;
    read();
  }

  node->ctype = nodfindctype(type_name);

  if (node->ctype == 0)
  {
    printf("Type %s:\n", type_name);
    panic("Unknown type!");
  }
  
  if (is_struct && (node->ctype->flags & NODE_FLAGS_TYPE_FLAGS_STRUCT) == 0)
  {
    panic("Not a struct");
  }

  read();

  if (token == '*')
  {
    is_pointer = TRUE;
    node->flags |= NODE_FLAGS_TYPEDECL_FLAGS_POINTER;
    read();
  }

  expect('n');
  
  node->text = tokcopys();

  read();

  /* []
     [329]
  */
  if (token == '[')
  {
    is_pointer = TRUE;
    node->flags |= NODE_FLAGS_TYPEDECL_FLAGS_POINTER;

    read();

    if (token == 'i')
    {
      node->integer = tokgeti();
      node->flags |= NODE_FLAGS_TYPEDECL_FLAGS_INTEGERISIZE;
      read();
    }
    else
    {
      if (is_extern == FALSE)
      {
        panic("Size not given for array");
      }
    }

    expect(']');

    read();
  }

  /*
    ();
  */
  else if (token == '(')
  {
    is_function = TRUE;
    node->flags |= NODE_FLAGS_TYPEDECL_FLAGS_FUNCTION;

    if ((node->flags & NODE_FLAGS_TYPEDECL_FLAGS_POINTER) != 0)
    {
      panic("Function is marked as pointer");
    }

    read_and_expect(')');
    read();
  }

  expect(';');
}

nodrdscope(parent)
  struct Node* parent;
{
  extern int   token;
  struct Node* scope;
  int type_decl;

  scope = nodmk(NODE_TYPE_SCOPE, parent);
  expect('{');
  
  type_decl = (parent->flags & NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL) != 0;
  while(read() != 'X')
  {
    dont_expect('{');

    if (token == '}')
      break;
    
    /*
      asm("assembly");
      asnop
    */
    if (token == 'n')
    {
      if (tokchecks("asm"))
      {
        if (type_decl == 0)
        {
          type_decl = 1;
          parent->flags |= NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL;
        }
        nodrdasm(scope);
        continue;
      }
      else if (tokchecks("while"))
      {
        if (type_decl == 0)
        {
          type_decl = 1;
          parent->flags |= NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL;
        }
        nodrdwhile(scope);
        continue;
      }
      else if (tokchecks("int"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        nodrdtypedecl(scope);
        continue;
      }
      else if (tokchecks("char"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        nodrdtypedecl(scope);
        continue;
      }
      else if (tokchecks("struct"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        nodrdtypedecl(scope);
        continue;
      }
      else if (tokchecks("extern"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        nodrdtypedecl(scope);
        continue;
      }
      else
      {
        if (type_decl == 0)
        {
          type_decl = 1;
          parent->flags |= NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL;
        }
        nodrdasnop(scope);
      }
      continue;
    }
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
  struct Node* node = nodmk(NODE_TYPE_FUNCTION, cexterns);
  node->text = tokcopys();
  
  node->flags |= NODE_FLAGS_FUNCTION_FLAGS_DECLARED;
  
  read_and_expect('(');
  // @TODO argument parsing

  read_and_expect(')');
  
  // @TODO argument parsing
  node->flags |= NODE_FLAGS_FUNCTION_FLAGS_SCOPE;

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
  
  ctypes = nodmk(NODE_TYPE_CTYPES, 0);
  nodmkctype("char", 1);
  nodmkctype("int",  2);
  
  csymbols = nodmk(NODE_TYPE_SYMBOLS, 0);

  nodsymi("FALSE", CONSTANT_FALSE, 0);
  nodsymi("TRUE", CONSTANT_TRUE, 1);
  nodsymi("NO", CONSTANT_NO, 0);
  nodsymi("YES", CONSTANT_YES, 1);
  
  cexterns = nodmk(NODE_TYPE_FILE, 0);
  

}

nodstop()
{
  cfree(cexterns);
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

  nodprint(ctypes, 0);
  nodprint(csymbols, 0);
  nodprint(cexterns, 0);

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
