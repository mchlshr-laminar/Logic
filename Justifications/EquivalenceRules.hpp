#ifndef __EQUIV_RULES_H_
#define __EQUIV_RULES_H_

class EquivalenceRule;

#include "Justification.hpp"
#include "../Statements/StatementTree.hpp"
#include "../Statements/ProofStatement.hpp"
#include <utility>
#include <list>

typedef std::pair<StatementTree*, StatementTree*> equiv_pair;

class EquivalenceRule : public Justification
{
  private:
  //StatementTree form1, form2;
  std::list<equiv_pair> equivalent_pairs;
  
  bool areEquivalent(StatementTree* tree1, StatementTree* tree2);
  bool match(StatementTree* target, StatementTree* form, bind_map& binds);
  void matchFormOneNegation(StatementTree* target, equiv_pair& source);
  
  public:
  /*EquivalenceRule(const char* f1, const char* f2, const char* name) : Justification(name),
    form1(f1), form2(f2)
  {}*/
  EquivalenceRule(const char* name) : Justification(name)
  {}
  ~EquivalenceRule();
  
  void addEquivalentPair(const char* form1, const char* form2);
  
  bool isJustified(StatementTree& con, ant_list& ant);
};

#endif
