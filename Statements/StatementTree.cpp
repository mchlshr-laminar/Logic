#include "StatementTree.hpp"
#include <cstring>
#include <iostream>

using std::list;

//Parses the given string into a tree
StatementTree::StatementTree(const char* input) : is_affirmed(true)
{
  atom_name = new char[strlen(input)+1];
  strcpy(atom_name, input);
  stripParens(atom_name);
  //if strlen(atom_name) == 0 error
  int operatorPos = findOperator(atom_name);
  node_type = operatorType(atom_name[operatorPos]);
  
  //Create the children
  if(node_type != ATOM)
  {
    char* left = new char[operatorPos+1];
    char* right = new char[strlen(atom_name)-operatorPos];
    strncpy(left, atom_name, operatorPos);
    strncpy(right, atom_name+operatorPos+1, strlen(atom_name)-operatorPos-1);
    delete [] atom_name;
    atom_name = NULL;
    //If length of right == 0 error
    children.push_front(new StatementTree(right));
    delete [] right;
    if(strlen(left) != 0) children.push_front(new StatementTree(left));
    delete [] left;
    
    consolidateNegation();
    //consolidateChildren();
  }
}

//Copies the given tree. If dontNegate is true or not given, it will be
//a straight copy. Otherwise, it will add or remove a NOT node at the root.
StatementTree::StatementTree(StatementTree& other, bool dontNegate) : is_affirmed(dontNegate == other.is_affirmed)
{
  node_type = other.node_type;
  if(other.atom_name != NULL)
  {
    atom_name = new char[strlen(other.atom_name)+1];
    strcpy(atom_name, other.atom_name);
  }
  else atom_name = NULL;
  for(list<StatementTree*>::iterator itr = other.children.begin(); itr != other.children.end(); itr++)
    children.push_back(new StatementTree(**itr));
}

StatementTree::~StatementTree()
{
  if(atom_name != NULL)
  {
    delete [] atom_name;
    atom_name = NULL;
  }
}

//Incorporates negation into root
void StatementTree::consolidateNegation()
{
  if(node_type != NOT) return;
  
  StatementTree* old_child = children.front();
  children.splice(children.end(), old_child->children);
  node_type = old_child->node_type;
  is_affirmed = !old_child->is_affirmed;
  atom_name = old_child->atom_name;
  children.pop_front();
}

//Creates generalized conjunctions, disjunctions, and biconditionals when appropriate
//by removing children & adopting those children's children.
void StatementTree::consolidateChildren()
{  
  if(!isAssociative()) return;
  
  for(list<StatementTree*>::iterator itr = children.begin(); itr != children.end();)
  {
    if(node_type == (*itr)->node_type)
    {
      list<StatementTree*>::iterator old_child = itr++;
      children.splice(itr, (*old_child)->children);
      delete *old_child;
      itr = children.erase(old_child);
    }
    else itr++;
  }
}

child_itr StatementTree::begin()
{ return children.begin(); }

child_itr StatementTree::end()
{ return children.end(); }

bool StatementTree::isAffirmed()
{ return is_affirmed; }

int StatementTree::nodeType()
{ return node_type; }

char* StatementTree::atomName()
{ return atom_name; }

bool StatementTree::isAssociative()
{
  switch(node_type)
  {
    case IFF:
    case OR:
    case AND: return true;
    break;
    default: return false;
    break;
  }
}

bool StatementTree::isCommutative()
{
  switch(node_type)
  {
    case IFF:
    case OR:
    case AND: return true;
    break;
    default: return false;
    break;
  }
}

//Returns whether or not the given tree is the same as this tree.
bool StatementTree::equals(StatementTree& other)
{
  if(node_type != other.node_type) return false;
  if(is_affirmed != other.is_affirmed) return false;
  
  if(node_type == ATOM) return strcmp(atom_name, other.atom_name) == 0;
  
  list<StatementTree*>::iterator itr1 = children.begin();
  list<StatementTree*>::iterator itr2 = other.children.begin();
  for(; itr1 != children.end() && itr2 != other.children.end(); itr1++, itr2++)
    if(!(*itr1)->equals(**itr2)) return false;
  return itr1 == children.end() && itr2 == other.children.end();
}

//Allocates and returns the string form of this tree.
char* StatementTree::createDisplayString()
{
  char* result = NULL;
  if(node_type == ATOM)
  {
    result = new char[strlen(atom_name)+1];
    strcpy(result, atom_name);
  }
  else //Combine the children's strings
  {
    list<char*> inner;
    int inner_len = 0;
    //Find the strings for all children
    for(list<StatementTree*>::iterator itr = children.begin(); itr != children.end(); itr++)
    {
      inner.push_back((*itr)->createDisplayString());
      inner_len += strlen(inner.back());
    }
    
    if(node_type == NOT) //no parentheses, operator to the left
    {
      result = new char[strlen(inner.front())+2];
      strcpy(result, "!");
      strcat(result, inner.front());
    }
    else
    {
      inner_len += inner.size();
      result = new char[inner_len+inner.size()+2];
      strcpy(result, "(");
      int pos = 1;
      //Assemble the child strings
      for(list<char*>::iterator itr = inner.begin(); itr != inner.end(); itr++)
      {
        strcpy(result+pos, *itr);
        pos += strlen(*itr);
        result[pos] = typeOperator(node_type);
        pos++;
      }
      pos--;
      result[pos] = ')';
      result[pos+1] = '\0';
    }
    
    for(list<char*>::iterator itr = inner.begin(); itr != inner.end(); itr++)
      delete [] *itr;
  }
  
  //Add the "!" to the start if needed.
  if(!is_affirmed)
  {
    char* temp = new char[strlen(result)+2];
    strcpy(temp, "!");
    strcat(temp, result);
    delete [] result;
    result = temp;
  }
  
  return result;
}

void StatementTree::negate()
{ is_affirmed = !is_affirmed; }

//Removes any parentheses which enclose the whole string (i.e. ((a)) -> a;
//((a&c)|(b&c)) -> (a&c)|(b&c)
void StatementTree::stripParens(char* input)
{
  int strip_count = -1;
  int len = strlen(input);
  bool keep_going = true;
  while(keep_going)
  {
    strip_count++;
    int paren_depth = 0;
    if(input[len-strip_count-1] != ')') break;
    
    for(int i = strip_count; i < len-strip_count-1; i++)
    {
      if(input[i] == '(') paren_depth++;
      else if(input[i] == ')') paren_depth--;
      if(paren_depth < 0) return;
      if(paren_depth == 0)
      {
        keep_going = false;
        break;
      }
    }
  }
  
  input[len-strip_count] = '\0';
  len = strlen(input);
  for(int i = strip_count; i <= len; i++) input[i-strip_count] = input[i];
}

//Returns the position of the first instance of the lowest order-of-operations
//operator that appears outside of any parentheses (a&b&c -> 1, a&b|c -> 3,
//(a&b)&c -> 3), i.e. the breakpoint.
int StatementTree::findOperator(char* input) //Maybe look from right to left.
{
  int len = strlen(input);
  for(int type = OP_START; type <= OP_END; type++)
  {
    int paren_depth = 0;
    for(int i = 0; i < len; i++)
    {
      if(input[i] == '(') paren_depth++;
      else if(input[i] == ')') paren_depth--;
      else if(paren_depth == 0 && operatorType(input[i]) == type) return i;
    }
  }
  return strlen(input);
}

//Returns the integer code for the operation represented by the given character.
//If no operation is represented, returns the code for an atom.
int StatementTree::operatorType(char input)
{
  switch(input)
  {
  case '=': return IFF;
  break;
  case '>': return IMPLIES;
  break;
  case '|':
  case '+': return OR;
  break;
  case '&':
  case '^':
  case '*': return AND;
  break;
  case '!':
  case '~': return NOT;
  break;
  default: return ATOM;
  break;
  }
}

//Returns the default character used to represent the operation with the given
//integer code. If the given code represents no operation, return a space.
char StatementTree::typeOperator(int input)
{
  switch(input)
  {
  case IFF: return '=';
  break;
  case IMPLIES: return '>';
  break;
  case OR: return '|';
  break;
  case AND: return '&';
  break;
  case NOT: return '!';
  break;
  default: return ' ';
  break;
  }
}

void StatementTree::temp(int depth)
{
  for(int i = 0; i < depth; i++) std::cout << ' ';
  if(!is_affirmed) std::cout << '!';
  if(node_type != ATOM) std::cout << typeOperator(node_type) << '\n';
  else std::cout << atom_name << '\n';
  for(list<StatementTree*>::iterator itr = children.begin(); itr != children.end(); itr++)
    (*itr)->temp(depth+1);
}
