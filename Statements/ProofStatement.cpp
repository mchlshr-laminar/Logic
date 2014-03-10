#include "Statements/ProofStatement.hpp"
#include <iostream>
#include <sstream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::stringstream;
using std::string;

ProofStatement::ProofStatement(const char* input, bool is_assump) : parent(NULL),
   reason(NULL), is_assumption(is_assump), fail_type(NO_FAILURE)
{ data = new StatementTree(input); }

ProofStatement::ProofStatement(StatementTree* input, bool is_assump) : parent(NULL),
  reason(NULL), is_assumption(is_assump), fail_type(NO_FAILURE)
{ data = new StatementTree(*input); }

ProofStatement::~ProofStatement()
{ delete data; }

//Returns the sentence tree if this is a normal statement, null if this is a subproof
StatementTree* ProofStatement::getStatementData()
{ return data; }

//Returns the tree for the assumption of a subproof, null if this isnt a subproof
StatementTree* ProofStatement::getAssumption()
{ return NULL; }

//Returns true if this statement is a consequence of its antecedents by the stored
//rule. sets the fail_type flag if it is not justified.
bool ProofStatement::isJustified()
{
  if(!data->isValid())
  {
    fail_type = INVALID_STATEMENT;
    return false;
  }
  if(reason == NULL)
  {
    fail_type = NO_JUSTIFICATION;
    return false;
  }
  
  bool result = reason->isJustified(*data, antecedents);
  fail_type = (result)?NO_FAILURE:JUSTIFICATION_FAILURE;
  return result;
}

//Returns the reason this statement is not justified.
ProofStatement::failure_type_t ProofStatement::getFailureType()
{ return fail_type; }

Justification* ProofStatement::getJustification()
{ return reason; }

ProofStatement* ProofStatement::getParent()
{ return parent; }

//Returns a set of all statements which are a child of this statement.
statement_set* ProofStatement::getSubproofContents()
{ return NULL; }

int ProofStatement::getLineIndex()
{ return line_index; }

//Returns true iff this statement is a premise or an assumption for a
//subproof.
bool ProofStatement::isAssumption()
{ return is_assumption; }

//Allocates, creates, and returns a string which represents this statement
//(sentence, justification name, and antecedent line indices).
char* ProofStatement::createDisplayString()
{
  char* retval;
  stringstream result;
  
  char* statement_display = data->createDisplayString();
  result << statement_display << " ";
  delete [] statement_display;
  
  if(reason == NULL)
    result << "Not Justified";
  else
    result << reason->getName() << " ";
    
  ant_list::iterator itr = antecedents.begin();
  for(; itr != antecedents.end(); itr++)
    result << ((*itr)->getLineIndex()+1) << ", ";
  
  string temp = result.str();
  int len = (antecedents.size()>0)?(temp.size()-2):temp.size();
  retval = new char[len+1];
  strncpy(retval, temp.c_str(), len);
  retval[len] = '\0';
  return retval;
}

//Replaces this statement's sentence with the given input.
void ProofStatement::rewrite(const char* input)
{
  delete data;
  data = new StatementTree(input);
}

//Replaces this statement's sentence with the given input.
void ProofStatement::rewrite(StatementTree* input)
{
  delete data;
  data = new StatementTree(*input);
}

void ProofStatement::setJustification(Justification* new_reason)
{
  if(is_assumption && reason != NULL) return;
  reason = new_reason;
}

//Adds the given statement to the list of antecedents if it was not already
//there, removes it if it was. Returns true if it was removed.
bool ProofStatement::toggleAntecedent(ProofStatement* ant)
{
  ant = getRelevantAncestor(ant);
  
  if(ant == NULL) return false;
  for(ant_list::iterator itr = antecedents.begin(); itr != antecedents.end(); itr++)
  {
    if(*itr == ant) //Comparing addresses
    {
      antecedents.erase(itr);
      return true;
    }
  }
  antecedents.push_back(ant);
  return false;
}

//Makes this statement a child of the given parent by updating the parent
//pointer & updating the child sets of both new & old parents.
void ProofStatement::setParent(ProofStatement* new_parent)
{
  if(parent != NULL) parent->toggleChild(this);
  if(new_parent != NULL) new_parent->toggleChild(this);
  parent = new_parent;
}

void ProofStatement::setLineIndex(int i)
{ line_index = i; }

//Toggles whether or not the given statement is in the set of children
//Is private, only used by setParent
bool ProofStatement::toggleChild(ProofStatement* ch)
{ return false; }

//Returns the closest ancestor of the given statement which would be admissable
//as an antecedent of this statement (i.e. is not in a separate subproof).
ProofStatement* ProofStatement::getRelevantAncestor(ProofStatement* antecedent)
{
  if(antecedent == NULL) return NULL;
  if(antecedentAllowable(antecedent)) return antecedent;
  return getRelevantAncestor(antecedent->parent);
}

//Returns true if the parent of the given statement is an ancestor of this
//statement, i.e. it is not in a subproof which would disallow it from use
//as an antecedent
bool ProofStatement::antecedentAllowable(ProofStatement* ant)
{
  if(ant == NULL) return false;
  //if((*itr)->line_index >= line_index) return false; //The line was after this line
  
  ProofStatement* target = ant->parent;
  bool target_found = false;
  for(ProofStatement* traveller = parent; ; traveller = traveller->parent)
  {
    if(traveller == target)
    {
      target_found = true;
      break;
    }
    if(traveller == NULL) break;
  }
  if(!target_found) return false; //The line was in a subproof.
  return true;
}
