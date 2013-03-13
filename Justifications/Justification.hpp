#ifndef __JUSTIFICATION_H_
#define __JUSTIFICATION_H_

#include <map>
#include <list>

class Justification;
typedef std::map<char, StatementTree*> bind_map;

#include "../Statements/StatementTree.hpp"
#include "../Statements/ProofStatement.hpp"

class Justification
{
  private:
  char* rule_name;
  
  public:
  Justification(const char* name);
  ~Justification();
  
  virtual bool isJustified(StatementTree& con, ant_list& ant) = 0;
  char* getName();
};

#endif

