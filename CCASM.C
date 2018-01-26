/*
  cc -- C Compiler for DX8A Computer
*/

FILE* f;
char  temp[256];

emit(text, x1, x2, x3, x4, x5)
  char* text;
{
  fprintf(f, text, x1, x2, x3, x4, x5);
  fprintf(f, "\n");
}

asmscope(node)
  struct Node* node;
{
  struct Node* statement;

  statement = node->first;
  while(statement != 0)
  {
    if (statement->type == NODE_TYPE_ASSEMBLY)
    {
      emit("%s", statement->text);
    }
    statement = statement->next;
  }

}

asmfn(node)
  struct Node* node;
{
  struct Node* statement;

  emit("%s:", node->text);

  statement = node->first;
  while(statement != 0)
  {
    printf("%i", statement->type);

    if (statement->type == NODE_TYPE_SCOPE)
    {
      asmscope(statement);
    }
    statement = statement->next;
  }

  emit("__%s_end: ret", node->text);
}

asm()
{
  struct Node* node;
  f = fopen("out.s", "wb");

  node = cexterns->first;
  while(node != 0)
  {
    if (node->type == NODE_TYPE_FUNCTION)
    {
      asmfn(node);
    }
    node = node->next;
  }
  fclose(f);
}
