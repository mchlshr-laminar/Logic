#ifndef __JUSTIFICATION_H_
#define __JUSTIFICATION_H_

#include <map>
#include <list>

class Justification;

#include "../Statements/StatementTree.hpp"
#include "../Statements/ProofStatement.hpp"

typedef std::map<char, StatementTree*> bind_map;

class Justification
{
  private:
  char* rule_name;
  
  public:
  Justification(const char* name);
  ~Justification();
  
  virtual bool isJustified(StatementTree& con, ant_list& ant) = 0;
  char* getName();
  
  protected:
  void removeBoundForms(bind_map& binds);
  void removeNewlyBoundForms(bind_map& new_binds, bind_map& old_binds);
};

#endif

