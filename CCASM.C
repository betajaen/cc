/*
  cc -- C Compiler for DX8A Computer
*/

FILE* asmfile;

char base62[62] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
  'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
  'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
  'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
  'u', 'v', 'w', 'x', 'y', 'z'
};

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

  fprintf(asmfile, "%2X%s", strlen(name), name);

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
    typedecl = node_find_type(arg, NODE_TYPE_TYPEDECL);

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
      fprintf(asmfile, "S%X", typedecl->ctype->num_value);
    }

    arg = arg->next;
  }


}

eval_is_const_condition(node)
  struct Node* node;
{
  if (node == 0)
  {
    return TRUE;
  }

  if (node->type == NODE_TYPE_NUMBER)
  {
    return TRUE;
  }
  
  return FALSE;
}

eval_const_condition(node)
  struct Node* node;
{
  if (node == 0)
  {
    return FALSE;
  }
  
  if (node->type == NODE_TYPE_NUMBER)
  {
    return node->num_value != 0;
  }

  return FALSE;
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
      int write_while;
      int write_runtime;

      write_while = TRUE;
      write_runtime = TRUE;
      cond = statement->while_cond;
      
      if (eval_is_const_condition(statement->if_cond))
      {
        write_while = eval_const_condition(statement->if_cond);
        write_runtime = FALSE;
      }

      if (write_while)
      {
        label("L%i", statement->index);

        if (write_runtime == TRUE)
        {
          /* write runtime condition and branch here */
        }
        
        asmscope(statement->while_scope, statement);

        instruction("BRA L%i", statement->index);
        label("L%i_END", node->index);
      }

    }
    else if (statement->type == NODE_TYPE_IF)
    {
      struct Node* cond;
      int write_then, write_else;

      write_then = 1;
      write_else = 1;

      if (eval_is_const_condition(statement->if_cond))
      {
        if (eval_const_condition(statement->if_cond))
        {
          write_then = TRUE;
          write_else = FALSE;
        }
        else
        {
          write_then = FALSE;
          write_else = TRUE;
        }
      }
      else
      {
        /* TODO Write runtime if condition */
      }

      if (write_then == TRUE)
      {
        if (write_else == TRUE)
        {
          label("L%i_THEN", statement->index);
        }
        asmscope(statement->if_then, statement);
      }

      if (write_else == TRUE)
      {
        if (write_then == TRUE)
        {
          label("L%i_ELSE", statement->index);
        }
        asmscope(statement->if_else, statement);
      }

      if (write_then == TRUE && write_then == TRUE)
      {
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
      args = node_find_type(node, NODE_TYPE_ARGUMENT_LIST);
      
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

  args = node_find_type(node, NODE_TYPE_ARGUMENT_LIST);
  
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
