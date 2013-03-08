#ifndef __EQUIV_RULES_H_
#define __EQUIV_RULES_H_

//#include "Justification.hpp"
#include "../Statements/StatementTree.hpp"
#include <map>

typedef std::map<char, StatementTree*> bind_map;

class EquivalenceRule //: public Justification
{
  protected:
  StatementTree form1, form2; //Tree has no default constructor, use pointers?
  
  bool match(StatementTree* target, StatementTree* form, bind_map& binds);
  void matchFormOneNegation(StatementTree* target);
  
  public:
  EquivalenceRule(char* f1, char* f2) : form1(f1), form2(f2)
  {}
  
  //bool isJustified();
  
  bool areEquivalent(StatementTree* tree1, StatementTree* tree2);
};

#endif
