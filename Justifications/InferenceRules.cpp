#include "InferenceRules.hpp"
#include "SubProof.hpp"
#include <utility>
#include <iostream>

using std::list;
using std::pair;
using std::cout;
using std::cerr;
using std::endl;

InferenceRule::~InferenceRule()
{
  list<form_pair>::iterator itr = required_forms.begin();
  for(; itr != required_forms.end(); itr++)
  {
    delete FORM_STATEMENT(*itr);
    if(FORM_ASSUMPTION(*itr) != NULL) delete FORM_ASSUMPTION(*itr);
  }
}

//Adds a statement form that some antecedent must match for this inference rule
//to be used.
void InferenceRule::addRequiredForm(const char* statement, const char* assumption)
{
  if(assumption == NULL)
    required_forms.push_back(form_pair(NULL, new StatementTree(statement)));
  else
  {
    required_forms.push_back(form_pair(new StatementTree(assumption),
      new StatementTree(statement)));
  }
}

//Returns true if and only if the given consequent & antecedents can be matched
//to this inference rule's consequent form & required antecedent forms with none
//of the given antecedents being unneccesary.
bool InferenceRule::isJustified(StatementTree& con, antecedent_list& ant)
{
  //If there are more antecedents than required, some will be unused.
  //It's possible one antecedent could satisfy multiple forms so equal size is
  //not required.
  if(ant.size() > required_forms.size()) return false;
  for(antecedent_list::iterator itr = ant.begin(); itr != ant.end(); itr++)
    if(*itr == NULL) return false;
  
  //Checking that the consequent is of the correct form.
  bind_map binds;
  if(!match(&con, &result_form, binds))
  {
    removeBoundForms(binds);
    return false;
  }
  
  //Initializing the map of antecedent usage, for determining if there are any
  //unused antecedents.
  statement_usage_map ant_usage;
  for(antecedent_list::iterator itr = ant.begin(); itr != ant.end(); itr++)
    ant_usage[*itr] = 0;
  
  bool result = findAntecedentsForForms(required_forms.begin(), ant, binds,
    ant_usage);
  removeBoundForms(binds);
  return result;
}

//Attempts to match the given required antecedent form to some actual antecedent,
//contingent on previously bound sentence variables. On a successful match,
//continues to the next required form. If all required forms have a corresponding
//antecedent, returns true iff there are no unused antecedents.
bool InferenceRule::findAntecedentsForForms(list<form_pair>::iterator form, 
  antecedent_list& ant, bind_map& binds, statement_usage_map& ant_usage)
{
  //At the end, check if any antecedents were unused.
  if(form == required_forms.end()) return checkAntecedentRelevance(ant_usage);
  
  if(FORM_ASSUMPTION(*form)==NULL)
    return findAntecedentsForBasicForm(form, ant, binds, ant_usage);
  else
    return findAntecedentsForSubProof(form, ant, binds, ant_usage);
}

//Helper function for findAntecedentsForForms for when the form is not a
//subproof form.
bool InferenceRule::findAntecedentsForBasicForm(list<form_pair>::iterator form, 
  antecedent_list& ant, bind_map& binds, statement_usage_map& ant_usage)
{
  //The copy of the variable binding map is so new bindings can be removed if
  //a branch doesn't work out.
  bind_map temp_binds(binds);
  list<form_pair>::iterator next_form = form;
  next_form++;
  
  //Try each antecedent for this form.
  for(antecedent_list::iterator itr = ant.begin(); itr != ant.end(); itr++)
  {
    if(*itr == NULL) return false;
    StatementTree* ant_data = (*itr)->getStatementData();
    if(ant_data == NULL) continue; //Statement was a subproof
    
    //Attempt to match
    bool result = match(ant_data, FORM_STATEMENT(*form), temp_binds);
    if(result)
    {
      //If successful, move on to the next required form.
      ant_usage[*itr]++;
      result = findAntecedentsForForms(next_form, ant, temp_binds, ant_usage);
      ant_usage[*itr]--;
    }
    
    removeNewlyBoundForms(temp_binds, binds);
    if(result) return true;
  }
  return false;
}

//Helper function for findAntecedentsForForms for a subproof form.
bool InferenceRule::findAntecedentsForSubProof(list<form_pair>::iterator form, 
  antecedent_list& ant, bind_map& binds, statement_usage_map& ant_usage)
{
  //The copy of the variable binding map is so new bindings can be removed if
  //a branch doesn't work out.
  bind_map statement_binds(binds);
  list<form_pair>::iterator next_form = form;
  next_form++;
  
  for(antecedent_list::iterator itr = ant.begin(); itr != ant.end(); itr++)
  {
    //Iterate over antecedents
    if(*itr == NULL) return false;
    StatementTree* assumption = (*itr)->getAssumption();
    statement_set* contents = (*itr)->getSubproofContents();
    if(assumption == NULL) continue; //Statement wasn't a subproof
    
    bool result = match(assumption, FORM_ASSUMPTION(*form), statement_binds);
    if(!result) //Assumption didn't match
    {
      removeNewlyBoundForms(statement_binds, binds);
      continue;
    }
    
    bind_map substatement_binds(statement_binds);
    statement_set::iterator sub_itr = contents->begin();
    result = false;
    for(; sub_itr != contents->end(); sub_itr++)
    {
      //Iterate over children of antecedents to find the required statement
      //within the subproof.
      StatementTree* sub_statement = (*sub_itr)->getStatementData();
      if(sub_statement == NULL) continue;
      
      result = match(sub_statement, FORM_STATEMENT(*form), substatement_binds);
      if(result)
      {
        ant_usage[*itr]++;
        result = findAntecedentsForForms(next_form, ant, substatement_binds,
          ant_usage);
        ant_usage[*itr]--;
      }
      
      removeNewlyBoundForms(substatement_binds, statement_binds);
      if(result) break;
    }
    
    removeNewlyBoundForms(statement_binds, binds);
    if(result) return true;
  }
  return false;
}

//Returns true if all stored antecedents have been used at least once, false
//otherwise.
bool InferenceRule::checkAntecedentRelevance(statement_usage_map& ant_usage)
{
  statement_usage_map::iterator itr = ant_usage.begin();
  for(; itr != ant_usage.end(); itr++)
    if(itr->second <= 0)
      return false;
  return true;
}

bool InferenceRule::match(StatementTree* target, StatementTree* form, bind_map& binds)
{
  //The form is a sentence variable; if it's unbound, bind & return. Else return whether
  //the target is equivalent to the bound sentence.
  if(form->nodeType() == StatementTree::ATOM)
  {
    StatementTree* sentence = new StatementTree(*target, form->isAffirmed());
    pair<bind_map::iterator, bool> retval = 
      binds.insert(pair<char, StatementTree*>(form->atomName()[0], sentence));
    
    if(retval.second) return true;
    else
    {
      bool result = sentence->equals(*(retval.first->second)); //Here's where it differs from Equiv
      delete sentence;
      return result;
    }
  }
  
  //The form is not a sentence variable, return true if target matches form's type &
  //affirmation & corresponding children also match.
  if(target->nodeType() != form->nodeType() || target->isAffirmed() != form->isAffirmed())
    return false;
  child_itr itr1 = target->begin();
  child_itr itr2 = form->begin();
  for(; itr1 != target->end(), itr2 != form->end(); itr1++, itr2++)
    if(!match(*itr1, *itr2, binds)) return false;
  return itr1 == target->end() && itr2 == form->end();
}

