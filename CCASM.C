/*
  cc -- C Compiler for DX8A Computer
*/

FILE* asmfile;

label(name, x1, x2, x3, x4, x5)
  char* name;
{
  fprintf(asmfile, name, x1, x2, x3, x4, x5);
  fprintf(asmfile, ":\n");
}

instruction(text, x1, x2, x3, x4, x5)
  char* text;
{
  fprintf(asmfile, "\t");
  fprintf(asmfile, text, x1, x2, x3, x4, x5);
  fprintf(asmfile, "\n");
}

text(text, x1, x2, x3, x4, x5)
  char* text;
{
  fprintf(asmfile, text, x1, x2, x3, x4, x5);
}


assembly(text)
  char* text;
{
  char ch;
  ch = *text;
  text++;

  if (ch != 0)
  {
    fprintf(asmfile, "\t");
  }

  while(ch != 0)
  {
    if (ch == '\n')
    {
      fprintf(asmfile, "\n\t");
    }

    if (tokisname(ch))
    {
      ch = toupper(ch);
    }
    
    fwrite(&ch, 1, 1, asmfile);
    
    ch = *text;
    text++;
  }

  ch = '\n';
  fwrite(&ch, 1, 1, asmfile);
}

mangle(name, args)
  char* name;
  struct Node* args;
{
  int count;
  struct Node* arg;

  fprintf(asmfile, "F%s", name);

  if (args == 0)
  {
    fprintf(asmfile, "_0");
    return;
  }

  arg = args->first;
  count = 0;
  while(arg != 0)
  {
    count++;
    arg = arg->next;
  }

  if (count == 0)
  {
    fprintf(asmfile, "_%X", count);
  }
  else
  {
    fprintf(asmfile, "_%X", count);
  }

  arg = args->first;
  while(arg != 0)
  {
    struct Node* typedecl;
    typedecl = nodfindtype(arg, NODE_TYPE_TYPEDECL);

    if (typedecl->ctype == ctype_char)
    {
      fprintf(asmfile, "I1");
    }
    else if (typedecl->ctype == ctype_int)
    {
      fprintf(asmfile, "I2");
    }
    else if (typedecl->ctype == ctype_register)
    {
      fprintf(asmfile, "RR");
    }
    else if ((typedecl->ctype->flags & NODE_FLAGS_TYPE_FLAGS_STRUCTUNION) != 0)
    {
      fprintf(asmfile, "S%X", typedecl->ctype->integer);
    }

    arg = arg->next;
  }


}

asmscope(node, parent)
  struct Node* node;
  struct Node* parent;
{
  struct Node* statement;

  if (node == 0)
  {
    return;
  }

  statement = node->first;
  while(statement != 0)
  {
    if (statement->type == NODE_TYPE_ASSEMBLY)
    {
      assembly(statement->text);
    }
    else if (statement->type == NODE_TYPE_LABEL)
    {
      label("L%i_%s", node->index, statement->first->text);
    }
    else if (statement->type == NODE_TYPE_GOTO)
    {
      instruction("JMP L%i_%s", node->index, statement->text);
    }
    else if (statement->type == NODE_TYPE_WHILE)
    {
      struct Node* cond;

      cond = statement->cond->first;
      if (cond != 0)
      {
        /* while(number) */
        if (cond->type == NODE_TYPE_NUMBER)
        {
          /* while(true) */
          if (cond->integer == 1)
          {
            label("L%i", node->index);
            asmscope(nodfindtype(statement, NODE_TYPE_SCOPE), node);
            instruction("JMP L%i", node->index);
          }
        }
      }
    }
    else if (statement->type == NODE_TYPE_IF)
    {
      struct Node* cond;

      cond = statement->cond->first;
      if (cond != 0)
      {
        /* always true */
        if (cond->type == NODE_TYPE_NUMBER)
        {
          /* fallthrough */
          if (cond->integer == 0) /* false */
          {
            if (statement->last != statement->first)
            {
              instruction("BRA L%i_ELSE", statement->index);
            }
            else
            {
              instruction("BRA L%i_END", statement->index);
            }
          }
          else /* true */
          {
            /* nothing, allow fallthrough. */
          }
        }

        /* if */
        label("L%i_TRUE", statement->index);
        asmscope(nodfindtype(statement, NODE_TYPE_SCOPE), node);

        if (statement->last != statement->first)
        {
          instruction("BRA L%i_END", statement->index);
        }

        /* else */
        if (statement->last != statement->first)
        {
          label("L%i_ELSE", statement->index);
          asmscope(statement->last, node);
        }
        
        label("L%i_END", statement->index);
      }
    }
    statement = statement->next;
  }
}

asmtable()
{
  struct Node* node;
  
  node = cexterns->first;
  while(node != 0)
  {
    
    if (node->type == NODE_TYPE_FUNCTION)
    {
      struct Node* args;
      args = nodfindtype(node, NODE_TYPE_ARGUMENT_LIST);
      
      text("; %s\n", node->text);
      text("db '");
      mangle(node->text, args);
      text("', 0\n");
      text("dw ");
      mangle(node->text, args);
      text("\n");
    }
    node = node->next;
  }
  text("db 0\n");
  text("dw 0\n");
  text("\n");
  text("\n");
}

asmfn(node)
  struct Node* node;
{
  struct Node* statement;
  struct Node* args;

  args = nodfindtype(node, NODE_TYPE_ARGUMENT_LIST);
  
  text("; %s\n", node->text);
  mangle(node->text, args);
  text(":\n");

  statement = node->first;
  while(statement != 0)
  {
    if (statement->type == NODE_TYPE_SCOPE)
    {
      asmscope(statement, node);
    }
    statement = statement->next;
  }

  label("L%s_END", node->text);
  instruction("ret");

  text("\n");
}

asm()
{
  struct Node* node;
  asmfile = fopen("out.s", "wb");

  asmtable();

  node = cexterns->first;
  while(node != 0)
  {
    if (node->type == NODE_TYPE_FUNCTION)
    {
      asmfn(node);
    }
    node = node->next;
  }
  fclose(asmfile);
}
