#ifndef __INFERENCE_RULES_H_
#define __INFERENCE_RULES_H_

class InferenceRule;

#include "Justification.hpp"
#include "../Statements/StatementTree.hpp"
#include "../Statements/ProofStatement.hpp"
#include <utility>
#include <list>

typedef std::pair<StatementTree*, StatementTree*> form_pair;
#define FORM_ASSUMPTION(x) (x).first
#define FORM_STATEMENT(x) (x).second

//Checks justification based on an inference rule
class InferenceRule : public Justification
{
  private:
  StatementTree result_form;
  std::list<form_pair> required_forms;
  
  bool match(StatementTree* target, StatementTree* form, bind_map& binds);
  
  bool findAntecedentsForForms(std::list<form_pair>::iterator form, ant_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);
  bool findAntecedentsForBasicForm(std::list<form_pair>::iterator form, ant_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);
  bool findAntecedentsForSubProof(std::list<form_pair>::iterator form, ant_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);
  
  bool checkAntecedentRelevance(statement_usage_map& ant_usage);
  
  public:
  InferenceRule(const char* result, const char* name) : Justification(name),
    result_form(result)
  {}
  
  virtual ~InferenceRule();
  
  void addRequiredForm(const char* statement, const char* assumption=NULL);
  
  bool isJustified(StatementTree& con, ant_list& ant);
};

#endif

