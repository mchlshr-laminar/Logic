#include "Justification.hpp"
#include "AggregateJustification.hpp"
#include <iostream>

using std::list;
using std::cerr;
using std::cout;
using std::endl;

AggregateJustification::~AggregateJustification()
{
  list<Justification*>::iterator itr = rules.begin();
  for(; itr != rules.end(); itr++)
    delete *itr;
}

//Checks if the given consequent is justified under any of the subrules.
bool AggregateJustification::isJustified(StatementTree& consequent, 
    antecedent_list& antecedents)
{
  list<Justification*>::iterator itr = rules.begin();
  for(; itr != rules.end(); itr++)
  {
    //Check each sub-rule in turn
    if((*itr)->isJustified(consequent, antecedents))
    {
      return true;
    }
  }
  return false;
}

//Adds a subrule to check for.
void AggregateJustification::addRule(Justification* new_rule)
{
  if(new_rule == NULL) return;
  rules.push_back(new_rule);
}

