/*
  cc -- C Compiler for DX8A Computer
*/

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
    case NODE_TYPE_POINTERTO:
      printf("+ %s->", node->text);
    break;
    case NODE_TYPE_ARRAYINDEX:
      printf("+ %s[]", node->text);
    break;
    case NODE_TYPE_FUNCTION:
      printf("+ %s()", node->text);
    break;
    case NODE_TYPE_SCOPE:
      printf("+ {}");
    break;
    case NODE_TYPE_NUMBER:
      printf("+ %i", node->num_value);
    break;
    case NODE_TYPE_REGISTER:
      printf("+ R%i", node->num_value);
    break;
    case NODE_TYPE_ASSEMBLY:
      printf("+ ASM %s", node->text);
    break;
    case NODE_TYPE_WHILE:
      printf("+ while");
      nodprint(node->while_cond, depth + 1);
    break;
    case NODE_TYPE_IF:
      printf("+ if");
      nodprint(node->if_cond, depth + 1);
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
      printf("+ Type '%s', Size=%i", node->text, node->type_size);
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
    case NODE_TYPE_ARGUMENT_LIST:
      printf("+ Argument List");
    break;
    case NODE_TYPE_ARGUMENT:
      printf("+ Argument '%s'", node->text);
    break;
    case NODE_TYPE_COPY: printf("+ =") ; break;
    case NODE_TYPE_ADD:  printf("+ +="); break;
    case NODE_TYPE_SUB:  printf("+ -="); break;
    case NODE_TYPE_MUL:  printf("+ *="); break;
    case NODE_TYPE_DIV:  printf("+ /="); break;
    case NODE_TYPE_MOD:  printf("+ %="); break;
    case NODE_TYPE_XOR:  printf("+ ^="); break;
    case NODE_TYPE_OR:   printf("+ |="); break;
    case NODE_TYPE_AND:  printf("+ &="); break;
    case NODE_TYPE_INC:  printf("+ ++"); break;
    case NODE_TYPE_DEC:  printf("+ --"); break;
    case NODE_TYPE_SHL:  printf("+ <<="); break;
    case NODE_TYPE_SHR:  printf("+ >>="); break;

  }
  
  printf("\n");
  
  if (node->ctype != 0)
  {
    nodprint(node->ctype, depth + 1);
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
  /*
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
  if (node->offset != 0)
  {
    i = 0;
    while(i < depth)
    {
      printf("-");
      i++;
    }
    printf("+ Offset: %i\n", node->offset);
  }
  */

  child = node->first;
  while(child != 0)
  {
    nodprint(child, depth + 1);
    child = child->next;
  }

}

void noddbg()
{
  nodprint(ctypes, 0);
  nodprint(csymbols, 0);
  nodprint(cexterns, 0);
}