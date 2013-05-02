#include "ProofStatement.hpp"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

ProofStatement::ProofStatement(const char* input) : parent(NULL), reason(NULL)
{ data = new StatementTree(input); }

ProofStatement::ProofStatement(StatementTree* input) : parent(NULL), reason(NULL)
{ data = new StatementTree(*input); }

StatementTree* ProofStatement::getStatementData()
{ return data; }

StatementTree* ProofStatement::getAssumption()
{ return NULL; }

bool ProofStatement::containsResult(StatementTree* match)
{ return false; }

bool ProofStatement::isJustified()
{
  if(!data->isValid() || reason == NULL /*|| !antecedentsAllowable()*/) return false;
  return reason->isJustified(*data, antecedents);
}

Justification* ProofStatement::getJustification()
{ return reason; }

ProofStatement* ProofStatement::getParent()
{ return parent; }

statement_set* ProofStatement::getSubproofContents()
{ return NULL; }

int ProofStatement::getLineIndex()
{ return line_index; }

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
  //Maybe bad.
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
