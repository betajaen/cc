/*
  cc -- C Compiler for DX8A Computer
*/

enum NodeType
{
  NODE_TYPE_NONE                        = 0 , 
  NODE_TYPE_FILE                        = 1 , /* <FILE>             */
  NODE_TYPE_EOF                         = 2 , /* <EOF>              */
  NODE_TYPE_SYMBOL                      = 3 , /* name               */
  NODE_TYPE_FUNCTION                    = 4 , /* m(x) int x; {}     */
  NODE_TYPE_SCOPE                       = 5 , /* { }                */
  NODE_TYPE_NUMBER                      = 6 , /* 49                 */
  NODE_TYPE_ASSEMBLY                    = 7 , /* asm("test");       */
  NODE_TYPE_WHILE                       = 8 , /* while(cond) {}     */
  NODE_TYPE_RETURN                      = 9 , /* return; return(name); return(4);      */
  NODE_TYPE_BREAK                       = 10, /* break;             */
  NODE_TYPE_STRING                      = 11, /* "string"           */
  NODE_TYPE_ASNOP                       = 12, /* NOT USED           */
  NODE_TYPE_TYPEDECL                    = 13, /* int x;             */
  NODE_TYPE_CTYPE                       = 14, /* int                */
  NODE_TYPE_CTYPES                      = 15, /* int, char          */
  NODE_TYPE_SYMBOLS                     = 16, /* name, name         */
  NODE_TYPE_STRUCT                      = 17, /* struct name { };   */
  NODE_TYPE_UNION                       = 18, /* union name { };    */
  NODE_TYPE_ARGUMENT_LIST               = 19, /* (x1, x2, x3, ...)  */
  NODE_TYPE_ARGUMENT                    = 20, /* x1, x2, x3, ...    */
  NODE_TYPE_REGISTER                    = 21, /* R0, R1, R2, ...    */
  NODE_TYPE_POINTERTO                   = 22, /* a->b               */
  NODE_TYPE_ARRAYINDEX                  = 23, /* a[3]               */
  NODE_TYPE_ARRAYSYMBOL                 = 24, /* a[name]            */
  NODE_TYPE_GOTO                        = 25, /* goto label         */
  NODE_TYPE_LABEL                       = 26, /* label:             */
  NODE_TYPE_IF                          = 27, /* if (expression) {} */
  
  NODE_TYPE_COPY                        = 30, /* a =  b;            */
  NODE_TYPE_ADD                         = 31, /* a += b;            */
  NODE_TYPE_SUB                         = 32, /* a -= b;            */
  NODE_TYPE_MUL                         = 33, /* a *= b;            */
  NODE_TYPE_DIV                         = 34, /* a /= b;            */
  NODE_TYPE_MOD                         = 35, /* a %= b;            */
  NODE_TYPE_XOR                         = 36, /* a ^= b;            */
  NODE_TYPE_OR                          = 37, /* a |= b;            */
  NODE_TYPE_AND                         = 38, /* a &= b;            */
  NODE_TYPE_INC                         = 39, /* a ++;              */
  NODE_TYPE_DEC                         = 40, /* a --;              */
  NODE_TYPE_SHL                         = 41, /* a <<=              */
  NODE_TYPE_SHR                         = 42, /* a >>=              */
};

enum TypeType
{
  TYPE_INT,
  TYPE_CHAR,
  TYPE_REGISTER,
  TYPE_STRUCT,
  TYPE_UNION
};

#define SCOPE_TYPE_FUNCTION                     0
#define SCOPE_TYPE_IF                           1
#define SCOPE_TYPE_WHILE                        2

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
  enum NodeType        type;
  int                  flags;
  char*                text;
  int                  index;

  struct Node*         first;
  struct Node*         last;
  struct Node*         next;
  struct Node*         prev;

  struct Node*         ctype;
  
  union
  {
    int            num_value;
    char*          str_value;
    struct
    {
      struct Node* fun_arguments;
      struct Node* fun_scope;
      struct Type* fun_return;
    };
    struct
    {
      struct Node* asnop_left;
      struct Node* asnop_right;
    };
    struct
    {
      struct Node* if_cond;
      struct Node* if_then;
      struct Node* if_else;
    };
    struct
    {
      struct Node* while_cond;
      struct Node* while_scope;
    };
    struct
    {
      enum TypeType type_type;
      int           type_size;
      int           type_offset;
    };
  };
};

int nodeidx;
struct Node* cexterns;
struct Node* ctypes;
struct Node* csymbols;

struct Node* csym_true;
struct Node* csym_false;

struct Node* ctype_char;
struct Node* ctype_int;
struct Node* ctype_short;
struct Node* ctype_register;

int token;

node_add(parent, node)
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

node_find(node, text)
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

node_find_type(node, type)
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

node_last_type(node)
  struct Node* node;
{
  if (node != 0 && node->last != 0)
  {
    return node->last->type;
  }

  return NODE_TYPE_NONE;
}

struct Node* node_make(type, parent)
  int type;
  struct Node* parent;
{
  extern int nodeidx;

  struct Node* node;
  node = calloc(1, sizeof(struct Node));
  zero(node, sizeof(struct Node));
  node->index = nodeidx++;
  node->type  = type;

  node_add(parent, node);
  return(node);
}

struct Node* type_make(type, name, size)
 enum TypeType type;
 char* name;
 int size;
{
  struct Node* ctype;
  ctype = calloc(1, sizeof(struct Node));
  zero(ctype, sizeof(struct Node));
  ctype->index = nodeidx++;

  ctype->type = NODE_TYPE_TYPEDECL;
  ctype->type_type = type;
  ctype->text = name;
  ctype->type_size = size;

  node_add(ctypes, ctype);

  return ctype;
}

type_find(name)
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

node_find_symbol(name)
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
node_make_symboli(name, val)
  char* name;
  int val;
{
  struct Node* node;
  node = node_make(NODE_TYPE_NUMBER, csymbols);
  node->text = name;
  node->num_value = val;
  node->ctype = ctype_int;

  return node;
}

/* node register symbol */
node_make_symbolr(name, val)
  char* name;
  int val;
{
  struct Node* node;
  node = node_make(NODE_TYPE_REGISTER, csymbols);
  node->text = name;
  node->num_value = val;
  node->ctype = ctype_register;
}

/* node register symbol */
node_make_symbols(name, val)
  char* name;
  char* val;
{
  struct Node* node;
  node = node_make(NODE_TYPE_STRING, csymbols);
  node->text = name;
  node->str_value = val;
  node->ctype = ctype_char;
}

/* node read assign operator right param */
node_read_right_value(parent)
  struct Node* parent;
{
  struct Node* right;
  read();

  if (token == 'i')
  {
    right = node_make(NODE_TYPE_NUMBER, parent);
    right->num_value = tokgeti();
  }
  else if (token == 'n')
  {
    right = node_make(NODE_TYPE_SYMBOL, parent);
    right->text = tokcopys();
  }
  else
  {
    panic("Unknown right token type");
  }
}

/* node read while */
node_read_while(parent)
  struct Node* parent;
{
  struct Node* while_;
  struct Scope* scope;

  while_ = node_make(NODE_TYPE_WHILE, parent);
  
  read_and_expect('(');
  read();

  /* while(TRUE)
     while(FALSE)
     while(any_symbol)
  */
  if (token == 'n')
  {
    while_->while_cond = node_find_symbol(tokgets());
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
      while_->while_cond = node_find_symbol("FALSE");
    }
    else
    {
      while_->while_cond = node_find_symbol("TRUE");
    }
  }
  else
  {
    panic("Unexpected symbol for while");
  }
  
  read_and_expect(')');
  read_and_expect('{');
  while_->while_scope = node_read_scope(while_);
}

/* node read if */
node_read_if(parent)
  struct Node* parent;
{
  struct Node* if_;
  struct Scope* scope;

  if (token == 'n')
  {
    if (tokchecks("if"))
    {
      if_ = node_make(NODE_TYPE_IF, parent);
      
      read_and_expect('(');
      read();

      /* while(TRUE)
         while(FALSE)
         while(any_symbol)
      */
      if (token == 'n')
      {
        if_->if_cond = node_find_symbol(tokgets());
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
          if_->if_cond = node_find_symbol("FALSE");
        }
        else
        {
          if_->if_cond = node_find_symbol("TRUE");
        }
      }
      else
      {
        panic("Unexpected symbol for if");
      }
  
      read_and_expect(')');
      read_and_expect('{');
      
      if_->if_then = node_read_scope(if_);
      return if_;
    }
    else
    {
      if_ = parent;
      read_and_expect('{');
      if_->if_else = node_read_scope(if_);
      return if_;
    }
  }

  panic("Unknown if/else syntax!");
}



/*
   name
   name->name2
*/
struct Node* node_read_left_value(parent)
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

    primary = node_make(NODE_TYPE_POINTERTO, parent);
    primary->text = name;

    identifier = node_read_left_value(primary);
  }
  else if (token == '[')
  {
    read();

    if (token == 'i')
    {
      primary = node_make(NODE_TYPE_ARRAYINDEX, parent);
      primary->num_value = tokgeti();

      read();

      expect(']');
      
      read();
    }
    else if (token == 'n')
    {
      primary = node_make(NODE_TYPE_ARRAYSYMBOL, parent);
      node_read_left_value(primary);

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
    primary = node_make(NODE_TYPE_SYMBOL, parent);
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
node_read_assign_op(parent)
  struct Node* parent;
{
  struct Node* node;
  struct Node* left;
  
  node = node_make(NODE_TYPE_NONE, parent);
  left = node_read_left_value(node);

  switch(token)
  {
    case '=':
    {
      node->type = NODE_TYPE_COPY;
      node_read_right_value(node);
    }
    break;
    case '++':
    {
      node->type = NODE_TYPE_INC;
    }
    break;
    case '+=':
    {
      node->type = NODE_TYPE_ADD;
      node_read_right_value(node);
    }
    break;
    case '-=':
    {
      node->type = NODE_TYPE_SUB;
      node_read_right_value(node);
    }
    break;
    case '--':
    {
      node->type = NODE_TYPE_DEC;
    }
    break;
    case '/=':
    {
      node->type = NODE_TYPE_DIV;
      node_read_right_value(node);
    }
    break;
    case '*=':
    {
      node->type = NODE_TYPE_MUL;
      node_read_right_value(node);
    }
    break;
    case '%=':
    {
      node->type = NODE_TYPE_MOD;
      node_read_right_value(node);
    }
    break;
    case '^=':
    {
      node->type = NODE_TYPE_XOR;
      node_read_right_value(node);
    }
    break;
    case '|=':
    {
      node->type = NODE_TYPE_OR;
      node_read_right_value(node);
    }
    break;
    case '&=':
    {
      node->type = NODE_TYPE_AND;
      node_read_right_value(node);
    }
    break;
    case '<<=':
    {
      node->type = NODE_TYPE_SHL;
      node_read_right_value(node);
    }
    break;
    case '>>=':
    {
      node->type = NODE_TYPE_SHR;
      node_read_right_value(node);
    }
    break;
    case ':':
    {
      node->type = NODE_TYPE_LABEL;
    }
    return;
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
node_read_typedecl(scope, args)
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

  node       = node_make(NODE_TYPE_TYPEDECL, scope);

  expect('n');

  /*
    int  x;
    int* x;
    int  x[];
    int  x[4];
    struct test x;
    struct test* x;
    struct test  x[];
    struct test  y[4];
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

  node->ctype = type_find(type_name);

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
    arg = node_find(args, node->text);
    if (arg != 0)
    {
      node_add(arg, node);
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
      node->num_value = tokgeti();
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
node_read_args(parent)
  struct Node* parent;
{
  struct Node* node;
  struct Node* arg;
  char   expecting;

  node = node_make(NODE_TYPE_ARGUMENT_LIST, parent);

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

      arg = node_make(NODE_TYPE_ARGUMENT, node);
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
node_read_argvars(parent)
  struct Node* parent;
{
  struct Node* args;
  args = node_find_type(parent, NODE_TYPE_ARGUMENT_LIST);
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
    node_read_typedecl(parent, args);
  }
  printf("ARGS END\n");
}

/*  node read scope
*/
node_read_scope(parent)
  struct Node* parent;
{
  extern int   token;
  struct Node* scope;
  int type_decl;

  scope = node_make(NODE_TYPE_SCOPE, parent);
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

        struct Node* node;

        /* asm("instructions"); */
        read_and_expect('(');
        read_and_expect('s');
  
        node = node_make(NODE_TYPE_ASSEMBLY, scope);
        node->text = tokcopys();
  
        read_and_expect(')');
        read_and_expect(';');

        continue;
      }
      else if (tokchecks("goto"))
      {
        if (type_decl == 0)
        {
          type_decl = 1;
          parent->flags |= NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL;
        }
        
        /*
            goto name;
        */
        struct Node* node;
        
        read_and_expect('n');
        
        node = node_make(NODE_TYPE_GOTO, scope);
        node->text = tokcopys();

        read_and_expect(';');
        continue;
      }
      else if (tokchecks("while"))
      {
        if (type_decl == 0)
        {
          type_decl = 1;
          parent->flags |= NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL;
        }
        node_read_while(scope);
        continue;
      }
      else if (tokchecks("if"))
      {
        if (type_decl == 0)
        {
          type_decl = 1;
          parent->flags |= NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL;
        }
        node_read_if(scope);
        continue;
      }
      else if (tokchecks("else"))
      {
        struct Node* last;

        if (type_decl == 0)
        {
          type_decl = 1;
          parent->flags |= NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL;
        }

        if (node_last_type(scope) == NODE_TYPE_IF)
        {
          node_read_if(scope->last);
        }
        else
        {
          panic("Unwanted else");
        }
        continue;
      }
      else if (tokchecks("int"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        node_read_typedecl(scope, 0);
        continue;
      }
      else if (tokchecks("short"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        node_read_typedecl(scope, 0);
        continue;
      }
      else if (tokchecks("char"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        node_read_typedecl(scope, 0);
        continue;
      }
      else if (tokchecks("struct"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        node_read_typedecl(scope, 0);
        continue;
      }
      else if (tokchecks("union"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        node_read_typedecl(scope, 0);
        continue;
      }
      else if (tokchecks("extern"))
      {
        if (type_decl == 1)
        {
          panic("type declared after statements!");
        }
        node_read_typedecl(scope, 0);
        continue;
      }
      else
      {
        if (type_decl == 0)
        {
          type_decl = 1;
          parent->flags |= NODE_FLAGS_FUNCTION_FLAGS_TYPE_DECL;
        }
        node_read_assign_op(scope);
      }
      continue;
    }
  }

  return scope;
}

node_read_structunion()
{
  struct Node* struct_;
  struct Node* var;
  int is_struct;

  /* struct | union */
  is_struct = tokchecks("struct");
  
  /* name */
  read_and_expect('n');
  char* struct_name = tokcopys();

  if (is_struct)
  {
    struct_ = type_make(TYPE_STRUCT, struct_name, 0);
  }
  else
  {
    struct_ = type_make(TYPE_UNION, struct_name, 0);
  }

  struct_->flags |= NODE_FLAGS_TYPE_FLAGS_STRUCTUNION;

  /* { */
  read_and_expect('{');
  read();

  /* type-decl; */
  while(TRUE)
  {
    if (token == '};')
      break;
    expect('n');
    node_read_typedecl(struct_, 0);
    expect(';');
    read();
  }
  
  /* ); */
  expect('};');
  
  /* struct/union sizes/offsets. 
     struct is sum of var sizes, union is largest var size.
     struct vars offsets are sequential, union offsets are 0. */
  var = struct_->first;
  if (is_struct)
  {
    while(var != 0)
    {
      var->type_offset = struct_->type_size;       /* offset */
      struct_->type_size += var->ctype->type_size; /* struct size */
      var = var->next;
    }
  }
  else
  {
    while(var != 0)
    {
      if (struct_->type_size < var->ctype->type_size)
        struct_->type_size = var->ctype->type_size;
      var = var->next;
    }
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
  node = node_make(NODE_TYPE_FUNCTION, cexterns);
  node->text = tokcopys();
  
  node->flags |= NODE_FLAGS_FUNCTION_FLAGS_DECLARED;
  
  read_and_expect('(');

  read();

  if (token == 'n')
  {
    node_read_args(node);
  }

  expect(')');
  
  read();

  if (token != '{')
  {
    node_read_argvars(node);
  }

  expect('{');

  node->flags |= NODE_FLAGS_FUNCTION_FLAGS_SCOPE;

  node_read_scope(node);
}

node_init()
{
  ctypes         = node_make(NODE_TYPE_CTYPES, 0);
  ctype_register = type_make(TYPE_REGISTER, "register", 2);
  ctype_char     = type_make(TYPE_CHAR,     "char",     1);
  ctype_int      = type_make(TYPE_INT,      "int",      2);
  ctype_short    = type_make(TYPE_INT,      "short",    2);
  
  csymbols = node_make(NODE_TYPE_SYMBOLS, 0);

  csym_false     = node_make_symboli("FALSE",0);
  csym_true      = node_make_symboli("TRUE", 1);

  cexterns = node_make(NODE_TYPE_FILE, 0);
}

node_stop()
{
  cfree(cexterns);
}

node_read_file()
{
  while(TRUE)
  {
    int token;
    
    token = read();

    switch(token)
    {
      case 'X':
        return;
      case 'n':
      {
        if (tokchecks("struct") || tokchecks("union"))
        {
          node_read_structunion();
          break;
        }
        nodrdfun();
      }
      break;
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
