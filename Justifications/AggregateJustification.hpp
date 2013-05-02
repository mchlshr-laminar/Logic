#ifndef __AGGREGATE_JUSTIFICATION_H_
#define __AGGREGATE_JUSTIFICATION_H_

#include "Justification.hpp"
#include <list>

class AggregateJustification : public Justification
{
  private:
  std::list<Justification*> rules;
  
  public:
  AggregateJustification(const char* name) : Justification(name)
  {}
  ~AggregateJustification();
  
  bool isJustified(StatementTree& con, ant_list& ant);
  
  void addRule(Justification* new_rule);
};

#endif

