#include "StatementTree.hpp"
#include <cstring>
#include <iostream>

using std::list;

//Parses the given string into a tree
StatementTree::StatementTree(const char* input) : is_affirmed(true), validity(VALIDITY_UNKNOWN)
{
  //Copy the input to modify it during parsing
  atom_name = new char[strlen(input)+1];
  strcpy(atom_name, input);

  //Remove any parentheses that enclose the entire string, then find the top-
  //level operator.
  stripParens(atom_name);
  int operator_pos = findOperator(atom_name);
  node_type = operatorType(atom_name[operator_pos]);
  if(node_type == ATOM) return; //No operator found, this is an atomic proposition
  
  //Extract the substrings for the child nodes
  char* left = new char[operator_pos+1];
  char* right = new char[strlen(atom_name)-operator_pos];
  strncpy(left, atom_name, operator_pos);
  left[operator_pos] = '\0'; //strncpy doesn't guarantee null character
  strcpy(right, atom_name+operator_pos+1);
  delete [] atom_name;
  atom_name = NULL; //This is not an atomic proposition
  //Check valid?
  
  //Parse the child nodes
  children.push_front(new StatementTree(right));
  if(strlen(left) != 0 && node_type != NOT) children.push_front(new StatementTree(left));
  
  //If this is a negation node, consolidate that into a negation flag so the
  //tree is binary.
  consolidateNegation();
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
  for(child_itr itr = begin(); itr != end(); itr++)
    delete *itr;
}

//Incorporates negation into root
void StatementTree::consolidateNegation()
{
  if(node_type != NOT) return;
  
  //Should only have one child. Make that node's children our children.
  StatementTree* old_child = children.front();
  children.splice(children.end(), old_child->children);

  //Acquire the old child's node type, and the inverse of its negation flag.
  //Note that the child shouldn't also be a negation node, as it will have
  //also called consolidateNegation during parsing.
  node_type = old_child->node_type;
  is_affirmed = !old_child->is_affirmed;
  
  if(old_child->atom_name != NULL)
  {
    //Old child was an atom, acquire its name.
    atom_name = new char[strlen(old_child->atom_name)+1];
    strcpy(atom_name, old_child->atom_name);
  }
  
  //Remove the old child
  children.pop_front();
}

//Creates generalized conjunctions, disjunctions, and biconditionals when appropriate
//by removing children & adopting those children's children.
//Not currently used: generalized conjunction/disjunction would make application of
//inference rules more difficult.
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

//Returns an iterator to the start of the list of children
child_itr StatementTree::begin()
{ return children.begin(); }

//Returns an iterator to the end of the list of children
child_itr StatementTree::end()
{ return children.end(); }

//Returns true if there is no negation flag attached to this node
bool StatementTree::isAffirmed()
{ return is_affirmed; }

//Returns true if this statement is syntactically acceptable/well
//formed. Valid is a misnomer but is shorter.
bool StatementTree::isValid()
{
  //If this has already been checked, return the result.
  if(validity == IS_VALID) return true;
  else if(validity == IS_INVALID) return false;
  
  //Is an atom, check for disallowed characters in name.
  if(node_type == ATOM)
  {
    if(atom_name == NULL || strlen(atom_name) == 0)
    {
      validity = IS_INVALID;
      return false;
    }
    for(unsigned int i = 0; i < strlen(atom_name); i++)
      if(atom_name[i] == '(' || atom_name[i] == ')' || operatorType(atom_name[i]) != ATOM)
      {
        validity = IS_INVALID;
        return false;
      }
    validity = IS_VALID;
    return true;
  }
  
  //Not an atom, check presence & validity of children
  if(children.size() < 2)
  {
    //Negation is the only type with one child & negation
    //nodes shouldn't exist after tree creation.
    validity = IS_INVALID;
    return false;
  }
  for(child_itr itr = begin(); itr != end(); itr++)
    if(!(*itr)->isValid())
    {
      validity = IS_INVALID;
      return false;
    }
  validity = IS_VALID;
  return true;
}

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
  //Node type and negation flag must match
  if(node_type != other.node_type) return false;
  if(is_affirmed != other.is_affirmed) return false;
  
  //If the nodes are atoms, they must have the same name.
  if(node_type == ATOM) return strcmp(atom_name, other.atom_name) == 0;
  
  //Otherwise their corresponding children must be equal.
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
  if(node_type == ATOM) //Display the atom name
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
  //Check for empty/null string
  if(input == NULL) return;
  int len = strlen(input);
  if(len == 0) return;
  
  int strip_count = 0;
  while(true)
  {
    if(input[0] != '(') break;
    
    int paren_depth = 1;
    int close_index;
    for(close_index = strip_count+1; close_index < len; close_index++)
    {
      if(input[close_index] == '(') paren_depth++;
      else if(input[close_index] == ')') paren_depth--;
      
      if(paren_depth == 0) break;
    }
    
    if(close_index != len-strip_count-1) break;
    else strip_count++;
  }
  
  input[len-strip_count] = '\0';
  for(int i = strip_count; i <= len-strip_count; i++)
    input[i-strip_count] = input[i];
}

//Returns the position of the first instance of the lowest order-of-operations
//operator that appears outside of any parentheses (a&b&c -> 1, a&b|c -> 3,
//(a&b)&c -> 3), i.e. the breakpoint.
int StatementTree::findOperator(char* input) //Maybe look from right to left.
{
  int len = strlen(input);
  for(int type = OP_START; type <= OP_END; type++)
  {
    //Iterate through order of operations
    int paren_depth = 0;
    for(int i = len-1; i >= 0; i--)
    {
      //Operator can't be within parentheses
      if(input[i] == ')') paren_depth++;
      else if(input[i] == '(') paren_depth--;
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

//For printing the sentence in a tree format. Not currently used except
//for debugging.
void StatementTree::DrawDebugGraph(int depth)
{
  for(int i = 0; i < depth; i++) std::cout << ' ';
  if(!is_affirmed) std::cout << '!';
  if(node_type != ATOM) std::cout << typeOperator(node_type) << '\n';
  else std::cout << atom_name << '\n';
  for(list<StatementTree*>::iterator itr = children.begin(); itr != children.end(); itr++)
    (*itr)->DrawDebugGraph(depth+1);
}
