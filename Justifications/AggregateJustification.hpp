#ifndef __AGGREGATE_JUSTIFICATION_H_
#define __AGGREGATE_JUSTIFICATION_H_

#include "Justification.hpp"
#include <list>

//Stores a justification rule which has more than one possible format
//e.g. Addition can be a => a|b or a => b|a
class AggregateJustification : public Justification
{
  private:
  std::list<Justification*> rules;
  
  public:
  AggregateJustification(const char* name) : Justification(name)
  {}
  virtual ~AggregateJustification();
  
  bool isJustified(StatementTree& con, ant_list& ant);
  
  void addRule(Justification* new_rule);
};

#endif

