#ifndef __INFERENCE_RULES_H_
#define __INFERENCE_RULES_H_

class InferenceRule;

#include "Justification.hpp"
#include "../Statements/StatementTree.hpp"
#include "../Statements/ProofStatement.hpp"

class InferenceRule : public Justification
{
  private:
  StatementTree result_form;
  tree_list required_forms;
  
  bool match(StatementTree* target, StatementTree* form, bind_map& binds);
  //bool findFormsForAntecedents(ant_list::iterator begin, ant_list::iterator end,
  //  bind_map& binds);
  bool findAntecedentsForForms(tree_list::iterator form, ant_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);
  bool checkAntecedentRelevance(statement_usage_map& ant_usage);
  
  public:
  InferenceRule(const char* result, const char* name) : Justification(name),
    result_form(result)
  {}
  
  ~InferenceRule();
  
  void addRequiredForm(const char* req);
  
  bool isJustified(StatementTree& con, ant_list& ant);
};

#endif

