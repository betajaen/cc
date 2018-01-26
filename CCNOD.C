/*
  cc -- C Compiler for DX8A Computer
*/

#define NODE_TYPE_NONE                          0
#define NODE_TYPE_FILE                          1
#define NODE_TYPE_EOF                           2
#define NODE_TYPE_SYMBOL                        3
#define NODE_TYPE_FUNCTION                      4
#define NODE_TYPE_SCOPE                         5
#define NODE_TYPE_NUMBER                        6
#define NODE_TYPE_ASSEMBLY                      7
#define NODE_TYPE_WHILE                         8
#define NODE_TYPE_RETURN                        9
#define NODE_TYPE_BREAK                         10
#define NODE_TYPE_STRING                        11
#define NODE_TYPE_ASNOP                         12
#define NODE_TYPE_TYPEDECL                      13
#define NODE_TYPE_CTYPE                         14
#define NODE_TYPE_CTYPES                        15
#define NODE_TYPE_SYMBOLS                       16
#define NODE_TYPE_STRUCT                        17  /* struct name { };   */
#define NODE_TYPE_UNION                         18  /* union name { };    */
#define NODE_TYPE_ARGUMENT_LIST                 19  /* (x1, x2, x3, ...)  */
#define NODE_TYPE_ARGUMENT                      20  /* x1, x2, x3, ...    */
#define NODE_TYPE_REGISTER                      21  /* R0, R1, R2, ...    */
#define NODE_TYPE_POINTERTO                     22  /* a->b               */
#define NODE_TYPE_ARRAYINDEX                    23  /* a[3]               */

#define SCOPE_TYPE_FUNCTION                     0
#define SCOPE_TYPE_IF                           1
#define SCOPE_TYPE_WHILE                        2

#define ASNOP_COPY                              0 /* a =  b; */
#define ASNOP_ADD                               1 /* a += b; */
#define ASNOP_SUB                               2 /* a -= b; */
#define ASNOP_MUL                               3 /* a *= b; */
#define ASNOP_DIV                               4 /* a /= b; */
#define ASNOP_MOD                               5 /* a %= b; */
#define ASNOP_XOR                               6 /* a ^= b; */
#define ASNOP_OR                                7 /* a |= b; */
#define ASNOP_AND                               8 /* a &= b; */
#define ASNOP_INC                               9 /* a ++;   */
#define ASNOP_DEC                              10 /* a --;   */
#define ASNOP_SHL                              11 /* a <<=   */
#define ASNOP_SHR                              12 /* a >>=   */

#define KEYWORD_TYPE_INT                       0
#define KEYWORD_TYPE_CHAR                      1
#define KEYWORD_TYPE_SIZE                      2

#define NODE_FLAGS_FUNCTION_FLAGS_DECLARED     1
#define NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL    2
#define NODE_FLAGS_FUNCTION_FLAGS_SCOPE        4
#define NODE_FLAGS_FUNCTION_FLAGS_ARGS         8
#define NODE_FLAGS_TYPE_FLAGS_STRUCTUNION      16
#define NODE_FLAGS_TYPEDECL_FLAGS_EXTERN       32
#define NODE_FLAGS_TYPEDECL_FLAGS_POINTER      64
#define NODE_FLAGS_TYPEDECL_FLAGS_INTEGERISIZE 128
#define NODE_FLAGS_TYPEDECL_FLAGS_FUNCTION     256

extern panic();
extern read();
extern expect();
extern expect_name();
extern read_and_expect();
extern char* tokgetn();
extern int tokread();
extern char* tokcopys();
extern tokcopy2();
extern int tokchecks();

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
  short        integer;
  short        offset;
  int          flags;
};

int nodeidx;
struct Node* cexterns;
struct Node* ctypes;
struct Node* csymbols;

int token;

nodadd(parent, node)
 struct Node* parent;
 struct Node* node;
{
  
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
}

nodfind(node, text)
  struct Node* node;
  char* text;
{
  struct Node* child;
  child = node->first;
  while(child != 0)
  {
    if (strcmp(text, child->text) == 0)
    {
      return(child);
    }
    child = child->next;
  }
  return 0;
}

nodfindtype(node, type)
  struct Node* node;
  int type;
{
  struct Node* child;
  child = node->first;
  while(child != 0)
  {
    if (child->type == type)
    {
      return(child);
    }
    child = child->next;
  }
  return 0;
}

struct Node* nodmk(type, parent)
  int type;
  struct Node* parent;
{
  extern int nodeidx;

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
  node->offset    = 0;

  nodadd(parent, node);
  return(node);
}

struct Node* nodmkctype(name, size)
 char* name;
 int size;
{
  struct Node* node;
  node = nodmk(NODE_TYPE_CTYPE, ctypes);
  node->text = name;
  node->integer = size;
  return node;
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

/* node integer symbol */
nodsymi(name, val)
  char* name;
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

/* node register symbol */
nodsymr(name, val)
  char* name;
  int val;
{
  struct Node* node;
  node = nodmk(NODE_TYPE_SYMBOL, csymbols);
  node->text = name;
  
  struct Node* value;
  value = nodmk(NODE_TYPE_REGISTER, node);
  value->integer = val;
  value->ctype   = nodfindctype("register");
}

/* node read assembly */
nodrdasm(parent)
  struct Node* parent;
{
  struct Node* node;

  /* asm("instructions"); */
  expect_name("asm");
  read_and_expect('(');
  read_and_expect('s');
  
  node = nodmk(NODE_TYPE_ASSEMBLY, parent);
  node->text = tokcopys();
  
  read_and_expect(')');
  read_and_expect(';');
}

/* node read assign operator right param */
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

/* node read while */
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
   name
   name->name2
*/
struct Node* nodrdlvalue(parent)
  struct Node* parent;
{
  struct Node* primary;
  struct Node* identifier;
  char* name;

  expect('n');

  name = tokcopys();

  read();

  if (token == '->' || token == '.')
  {
    read();
    expect('n');

    primary = nodmk(NODE_TYPE_POINTERTO, parent);
    primary->text = name;

    identifier = nodrdlvalue(primary);
  }
  else if (token == '[')
  {
    read();

    if (token == 'i')
    {
      primary = nodmk(NODE_TYPE_ARRAYINDEX, parent);
      primary->text = name;
      identifier = nodmk(NODE_TYPE_NUMBER, primary);
      identifier->integer = tokgeti();
      
      read();

      expect(']');
      
      read();
    }
    else if (token == 'n')
    {
      primary = nodmk(NODE_TYPE_ARRAYINDEX, parent);
      primary->text = name;

      nodrdlvalue(primary);

      expect(']');

      read();
    }
    else
    {
      panic("Unknown synax for array index accessor");
    }
  }
  else
  {
    primary = nodmk(NODE_TYPE_SYMBOL, parent);
    primary->text = name;
  }

  return primary;
}

/*
   node read assign nop
   
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
  left = nodrdlvalue(node);

  switch(token)
  {
    case '=':
    {
      node->sub_type = ASNOP_COPY;
      nodrdasnopr(node);
    }
    break;
    case '++':
    {
      node->sub_type = ASNOP_INC;
    }
    break;
    case '+=':
    {
      node->sub_type = ASNOP_ADD;
      nodrdasnopr(node);
    }
    break;
    case '-=':
    {
      node->sub_type = ASNOP_SUB;
      nodrdasnopr(node);
    }
    break;
    case '--':
    {
      node->sub_type = ASNOP_DEC;
    }
    break;
    case '/=':
    {
      node->sub_type = ASNOP_DIV;
      nodrdasnopr(node);
    }
    break;
    case '*=':
    {
      node->sub_type = ASNOP_MUL;
      nodrdasnopr(node);
    }
    break;
    case '%=':
    {
      node->sub_type = ASNOP_MOD;
      nodrdasnopr(node);
    }
    break;
    case '^=':
    {
      node->sub_type = ASNOP_XOR;
      nodrdasnopr(node);
    }
    break;
    case '|=':
    {
      node->sub_type = ASNOP_XOR;
      nodrdasnopr(node);
    }
    break;
    case '&=':
    {
      node->sub_type = ASNOP_AND;
      nodrdasnopr(node);
    }
    break;
    case '<<=':
    {
      node->sub_type = ASNOP_SHL;
      nodrdasnopr(node);
    }
    break;
    case '>>=':
    {
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

/* nod read typedecl. 
   if args is not 0, it will read into an existing node in args, and add to the argument.
*/
nodrdtypedecl(scope, args)
  struct Node* scope;
  struct Node* args;
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

  expect('n');

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

  if (strcmp(type_name, "struct") == 0 || strcmp(type_name, "union") == 0)
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
  
  if (is_struct && (node->ctype->flags & NODE_FLAGS_TYPE_FLAGS_STRUCTUNION) == 0)
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

  if (args != 0)
  {
    struct Node* arg;
    arg = nodfind(args, node->text);
    if (arg != 0)
    {
      nodadd(arg, node);
    }
  }

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

/*  node read arguments 
    (x1, x2, ...)
*/
nodrdargs(parent)
  struct Node* parent;
{
  struct Node* node;
  struct Node* arg;
  char   expecting;

  node = nodmk(NODE_TYPE_ARGUMENT_LIST, parent);

  expecting = 'n';

  while(TRUE)
  {
    if (token != 'n' && token != ',')
    {
      printf("leave %c\n", token);
      return;
    }

    if (expecting != token)
    {
      panic("Unexpected token in argument list!");
    }

    if (expecting == 'n')
    {
      expecting = ',';

      arg = nodmk(NODE_TYPE_ARGUMENT, node);
      arg->text = tokcopys();
      printf("%s\n", arg->text);
      read();
      continue;
    }

    if (expecting == ',')
    {
      expecting = 'n';
      read();
      continue;
    }
  }
}

/*
   int  x1;
   int* x2;
*/
nodrdargvars(parent)
  struct Node* parent;
{
  struct Node* args;
  args = nodfindtype(parent, NODE_TYPE_ARGUMENT_LIST);
  printf("ARGS IS = %p\n", args);
  // @TODO.
  // Just a while loop of typedecls with a ;, until a {
  while(TRUE)
  {
    if (token == '{')
      break;
    if (token == ';')
    {
      read();
      if (token == '{')
      {
        break;
      }
    }
    expect('n');
    nodrdtypedecl(parent, args);
  }
  printf("ARGS END\n");
}

/*  node read scope
*/
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
    if (token == '{')
    {
      panic("Extra { in scope!");
    }

    if (token == '}')
    {
      break;
    }

    /*
      asm("assembly");
      asnop
      while(TRUE)  { }
      while(FALSE) { }
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
        nodrdtypedecl(scope, 0);
        continue;
      }
      else if (tokchecks("short"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        nodrdtypedecl(scope, 0);
        continue;
      }
      else if (tokchecks("char"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        nodrdtypedecl(scope, 0);
        continue;
      }
      else if (tokchecks("struct"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        nodrdtypedecl(scope, 0);
        continue;
      }
      else if (tokchecks("union"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        nodrdtypedecl(scope, 0);
        continue;
      }
      else if (tokchecks("extern"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        nodrdtypedecl(scope, 0);
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

nodrdstruct()
{
  struct Node* node;
  struct Node* var;
  
  expect_name("struct");

  read_and_expect('n');
  char* struct_name = tokcopys();

  node = nodmkctype(struct_name, 0);
  node->flags |= NODE_FLAGS_TYPE_FLAGS_STRUCTUNION;

  printf("name = %s", node->text);
  printf("@@1!\n");
  read_and_expect('{');
  printf("@@2!\n");
  read();

  while(TRUE)
  {
    if (token == '};')
      break;
    printf("struct.1\n");
    expect('n');
    printf("struct.2\n");
    nodrdtypedecl(node, 0);
    printf("struct.3\n");
    expect(';');
    printf("struct.4\n");
    read();
  }
  
    printf("struct.5\n");
  expect('};');
  
    printf("struct.6\n");
  /* calculate struct size */
  var = node->first;
  while(var != 0)
  {
    var->offset = node->integer;  /* offset */
    node->integer += var->ctype->integer; /* struct size */
    var = var->next;
  }

}

nodrdunion()
{
  struct Node* node;
  struct Node* var;
  
  expect_name("union");

  read_and_expect('n');
  char* struct_name = tokcopys();

  node = nodmkctype(struct_name, 0);
  node->flags |= NODE_FLAGS_TYPE_FLAGS_STRUCTUNION;
  read_and_expect('{');
  read();

  while(TRUE)
  {
    if (token == '}')
      break;
    printf("union.1\n");
    expect('n');
    printf("union.2\n");
    nodrdtypedecl(node, 0);
    expect(';');
    read();
  }

  expect('}');
  read_and_expect(';');

  /* calculate union size. The largest size */
  var = node->first;
  while(var != 0)
  {
    if (node->integer < var->ctype->integer)
      node->integer = var->ctype->integer;
    var = var->next;
  }

}

nodrdfun()
{
  /*
    In the forms of:
      main()
      {
        Scope
      }
      main(x1, x2, x3...)
        int x1;
        int x2;
        int x3;
        ...
      {
        Scope
      }
  */
  struct Node* node;
  node = nodmk(NODE_TYPE_FUNCTION, cexterns);
  node->text = tokcopys();
  
  node->flags |= NODE_FLAGS_FUNCTION_FLAGS_DECLARED;
  
  read_and_expect('(');

  read();

  if (token == 'n')
  {
    nodrdargs(node);
  }

  expect(')');
  
  read();

  if (token != '{')
  {
    nodrdargvars(node);
  }

  expect('{');

  node->flags |= NODE_FLAGS_FUNCTION_FLAGS_SCOPE;

  nodrdscope(node);
}

/* read declaration from global scope*/
nodrdglobdecl()
{
  if (tokchecks("struct"))
  {
    nodrdstruct();
    return;
  }
  
  if (tokchecks("union"))
  {
    nodrdunion();
    return;
  }

  // If it's not a keyword, then it's probably a function.
  nodrdfun();
}

nodinit()
{
  ctypes = nodmk(NODE_TYPE_CTYPES, 0);
  nodmkctype("register", 2);
  nodmkctype("char", 1);
  nodmkctype("int",  2);
  nodmkctype("short",  2);
  
  csymbols = nodmk(NODE_TYPE_SYMBOLS, 0);

  nodsymi("FALSE",0);
  nodsymi("TRUE", 1);
  nodsymi("NO",   0);
  nodsymi("YES",  1);

  nodsymr("R0", 0);
  nodsymr("R1", 1);
  nodsymr("R2", 2);
  nodsymr("R3", 3);
  nodsymr("R4", 4);
  nodsymr("R5", 5);
  nodsymr("R6", 6);
  nodsymr("R7", 7);
  nodsymr("R8", 8);
  nodsymr("R9", 9);
  nodsymr("R10",10);
  nodsymr("R11",11);
  nodsymr("R12",12);
  nodsymr("R13",13);
  nodsymr("R14",14);
  nodsymr("R15",15);
  
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

    // main() {}
    // struct name { type-decl-list };
    if (token == 'n')
    {
      nodrdglobdecl();
      continue;
    }
  }
}

expect(ch)
  int ch;
{
  extern int token;
  
  if (token == 'X')
  {
    panic("Unxpected End of File");
  }

  if (token != ch)
  {
    if (token == 'n')
    {
      printf("n = %s\n", tokgetn());
    }
    else if (token == 's')
    {
      printf("s = %s\n", tokgetn());
    }
    else if (token == 'i')
    {
      printf("i = %i\n", tokgeti());
    }
    panic("Unexpected token. Want a %c. Got %c.", ch, token);

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
  int ch;
{
  read();
  expect(ch);
}
