#include "ProofStatement.hpp"

ProofStatement::ProofStatement(const char* input) : reason(NULL)
{ data = new StatementTree(input); }

ProofStatement::ProofStatement(StatementTree* input) : reason(NULL)
{ data = new StatementTree(input); }

StatementTree* ProofStatement::getStatementData()
{ return data; }

StatementTree* ProofStatement::getAssumption()
{ return NULL; }

bool ProofStatement::containsResult(StatementTree* match)
{ return false; }

bool ProofStatement::isJustified()
{
  if(!data->isValid || reason == NULL || !antecedentsAllowable()) return false;
  return reason->isJustified(*data, antecedents);
}

bool ProofStatement::antecedentsAllowable()
{
  for(ant_list::iterator itr = antecedents.begin(); itr != antecedents.end(); itr++)
  {
    if(*itr == NULL) return false;
    //if((*itr)->line_index >= line_index) return false; //The line was after this line
    
    ProofStatement* target = (*itr)->parent;
    bool target_found = false;
    for(ProofStatement* traveller = parent; ; traveller = traveller->parent)
    {
      if(traveller == parent)
      {
        target_found = true;
        break;
      }
      if(traveller == NULL) break;
    }
    if(!target_found) return false; //The line was in a subproof.
  }
  return true;
}

ProofStatement* ProofStatement::getParent()
{ return parent; }

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
  data = new StatementTree(input);
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
  if(ant == NULL) return false;
  for(list<ProofStatement*>::iterator itr = antecedents.begin(); itr != antecedents.end(); itr++)
  {
    if(*itr == ant) //Comparing addresses
    {
      antecedents.erase(itr);
      return true;
    }
  }
  antecedents.push_back(itr);
  return false;
}

