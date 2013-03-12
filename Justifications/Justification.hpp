#ifndef __JUSTIFICATION_H_
#define __JUSTIFICATION_H_

#include "../Statements/StatementTree.hpp"
#include "../Statements/ProofStatement.hpp"
#include <list>

class Justification
{
  private:
  char* rule_name;
  
  public:
  Justification(const char* name);
  
  virtual bool isJustified(StatementTree& con, ant_list& ant) = 0;
  char* getName();
};

#endi

