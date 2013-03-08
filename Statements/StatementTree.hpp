#ifndef __STATEMENT_TREE_H_
#define __STATEMENT_TREE_H_

#include <list>

class StatementTree;
typedef std::list<StatementTree*>::iterator child_itr;

class StatementTree
{
  public:
  const static int ATOM = 0, IFF = 1, IMPLIES = 2, OR = 3, AND = 4, NOT = 5;
  const static int OP_START = 1, OP_END = 5;
  const static int ONLY = 0, LEFT = 0, RIGHT = 1;
  
  private:
  int node_type;
  std::list<StatementTree*> children;
  char* atom_name;
  bool is_affirmed;
  
  void consolidateNegation();
  void consolidateChildren();
  
  public:
  StatementTree(const char* input);
  StatementTree(StatementTree& other, bool dontNegate=true);
  ~StatementTree();
  
  //iteraton
  child_itr begin();
  child_itr end();
  
  //properties
  bool isAffirmed();
  int nodeType();
  char* atomName();
  bool isAssociative();
  bool isCommutative();
  bool equals(StatementTree& other);
  
  //display
  char* createDisplayString();
  void negate();
  
  static void stripParens(char* input);
  static int findOperator(char* input);
  static int operatorType(char input);
  static char typeOperator(int input);
  
  void temp(int depth = 0);
};

#endif
