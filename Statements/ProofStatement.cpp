#include "ProofStatement.hpp"
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

StatementTree* ProofStatement::getStatementData()
{ return data; }

StatementTree* ProofStatement::getAssumption()
{ return NULL; }

bool ProofStatement::containsResult(StatementTree* match)
{ return false; }

bool ProofStatement::isJustified()
{
  //if(!data->isValid() || reason == NULL /*|| !antecedentsAllowable()*/) return false;
  //return reason->isJustified(*data, antecedents);
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

int ProofStatement::getFailureType()
{ return fail_type; }

Justification* ProofStatement::getJustification()
{ return reason; }

ProofStatement* ProofStatement::getParent()
{ return parent; }

statement_set* ProofStatement::getSubproofContents()
{ return NULL; }

int ProofStatement::getLineIndex()
{ return line_index; }

bool ProofStatement::isAssumption()
{ return is_assumption; }

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

void ProofStatement::rewrite(const char* input)
{
  delete data;
  data = new StatementTree(input);
}

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

void ProofStatement::setParent(ProofStatement* new_parent)
{
  if(parent != NULL) parent->toggleChild(this);
  if(new_parent != NULL) new_parent->toggleChild(this);
  parent = new_parent;
}

void ProofStatement::setLineIndex(int i)
{ line_index = i; }

bool ProofStatement::toggleChild(ProofStatement* ch)
{ return false; }

ProofStatement* ProofStatement::getRelevantAncestor(ProofStatement* antecedent)
{
  if(antecedent == NULL) return NULL;
  if(antecedentAllowable(antecedent)) return antecedent;
  return getRelevantAncestor(antecedent->parent);
}

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
