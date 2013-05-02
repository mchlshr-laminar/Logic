#include "Justification.hpp"
#include "AggregateJustification.hpp"

using std::list;

AggregateJustification::~AggregateJustification()
{
  list<Justification*>::iterator itr = rules.begin();
  for(; itr != rules.end(); itr++)
    delete *itr;
}

bool AggregateJustification::isJustified(StatementTree& con, ant_list& ant)
{
  list<Justification*>::iterator itr = rules.begin();
  for(; itr != rules.end(); itr++)
    if((*itr)->isJustified(con, ant));
      return true;
  return false;
}

void AggregateJustification::addRule(Justification* new_rule)
{
  if(new_rule == NULL) return;
  rules.push_back(new_rule);
}

