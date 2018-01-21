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

struct Node;

struct Node
{
  int          node_type;
  int          sub_type;
  int          symbol;
  int          index;
  char*        text;
  int          text_length;
  struct Node* next;
  struct Node* prev;
  struct Node* first;
  struct Node* last;
  struct Node* return_;
  int          number;
};
