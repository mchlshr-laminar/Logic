#ifndef __INFERENCE_RULES_H_
#define __INFERENCE_RULES_H_

class InferenceRule;

#include "Justification.hpp"
#include "StatementTree.hpp"
#include "ProofStatement.hpp"
#include <utility>
#include <list>

/// <summary>
/// Stores the format which must be matched by one of a line's antecedents for
/// this rule to be applicable. If the rule requires a subproof, 
/// subproofAssumptionForm is the form that the assumption of that subproof
/// must have while statementForm is the form which must be found on some
/// line of the subproof. If the required form is not a subproof, 
/// subproofAssumptionForm will be null and statementForm has the form.
/// </summary>
struct required_form
{
  StatementTree* subproofAssumptionForm;
  StatementTree* statementForm;
};
typedef std::list<required_form*> required_form_list;

//Checks justification based on an inference rule
class InferenceRule : public Justification
{
  private:
  StatementTree result_form;
  required_form_list required_forms;
  
  bool match(StatementTree* target, StatementTree* form, bind_map& binds);
  
  bool findAntecedentsForForms(required_form_list::iterator form, antecedent_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);
  bool findAntecedentsForBasicForm(required_form_list::iterator form, antecedent_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);
  bool findAntecedentsForSubProof(required_form_list::iterator form, antecedent_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);
  
  bool checkAntecedentRelevance(statement_usage_map& ant_usage);
  
  public:
  InferenceRule(const char* result, const char* name) : Justification(name),
    result_form(result)
  {}
  
  virtual ~InferenceRule();
  
  void addRequiredForm(const char* statement, const char* assumption=NULL);
  
  bool isJustified(StatementTree& con, antecedent_list& ant);
};

#endif

