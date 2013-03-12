#ifndef __EQUIV_RULES_H_
#define __EQUIV_RULES_H_

//#include "Justification.hpp"
#include "../Statements/StatementTree.hpp"
#include <map>

typedef std::map<char, StatementTree*> bind_map;

class EquivalenceRule : public Justification
{
  private:
  StatementTree form1, form2;
  
  bool areEquivalent(StatementTree* tree1, StatementTree* tree2);
  bool match(StatementTree* target, StatementTree* form, bind_map& binds);
  void matchFormOneNegation(StatementTree* target);
  
  public:
  EquivalenceRule(char* f1, char* f2, const char* name) : form1(f1), form2(f2)
  { Justification(name); }
  
  bool isJustified(StatementTree& con, ant_list& ant)
};

#endif
