#ifndef __JUSTIFICATION_H_
#define __JUSTIFICATION_H_

#include <list>

class Justification;

#include "../Statements/StatementTree.hpp"
#include "../Statements/ProofStatement.hpp"

class Justification
{
  private:
  char* rule_name;
  
  public:
  Justification(const char* name);
  
  virtual bool isJustified(StatementTree& con, ant_list& ant) = 0;
  char* getName();
};

#endif

