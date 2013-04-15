#include "InferenceRules.hpp"
#include <utility>
#include <iostream>

using std::list;
using std::pair;
using std::cout;
using std::cerr;
using std::endl;

InferenceRule::~InferenceRule()
{
  for(tree_list::iterator itr = required_forms.begin(); itr != required_forms.end(); itr++)
    delete *itr;
}

//Adds a statement form that some antecedent must match for this inference rule
//to be used.
void InferenceRule::addRequiredForm(const char* req)
{ required_forms.push_back(new StatementTree(req)); }

//Returns true if and only if the given consequent & antecedents can be matched
//to this inference rule's consequent form & required antecedent forms with none
//of the given antecedents being unneccesary.
bool InferenceRule::isJustified(StatementTree& con, ant_list& ant)
{
  //If there are more antecedents than required, some will be unused.
  //It's possible one antecedent could satisfy multiple forms so equal size is
  //not required.
  if(ant.size() > required_forms.size()) return false;
  for(ant_list::iterator itr = ant.begin(); itr != ant.end(); itr++)
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
  for(ant_list::iterator itr = ant.begin(); itr != ant.end(); itr++)
    ant_usage[*itr] = 0;
  
  bool result = findAntecedentsForForms(required_forms.begin(), ant, binds,
    ant_usage);
  removeBoundForms(binds);
  return result;
}

//Attempts to match the given antecedent to some required form (contingent on
//previously bound sentence variables). On a successful match, continues on to
//the next antecedent (including any newly bound variables). If all antecedents
//can be matched to required forms with one set of variable bindings, returns
//true
/*bool InferenceRule::findFormsForAntecedents(ant_list::iterator begin,
  ant_list::iterator end, bind_map& binds)
{
  if(begin == end) return true;
    
  bind_map temp_binds(binds);
  
  tree_list::iterator form = required_forms.begin();
  for(; form != required_forms.end(); form++)
  {
    if(*form == NULL) return false;
    
    bool result = match((*begin)->getStatementData(), *form, temp_binds);
    ant_list::iterator next = begin;
    next++;
    result = result && findFormsForAntecedents(next, end, temp_binds);
    removeNewlyBoundForms(temp_binds, binds);
    if(result) return true;
  }
  return false;
}*/
//MUST REQUIRE ALL FORMS TO BE MATCHED

//Attempts to match the given required antecedent form to some actual antecedent,
//contingent on previously bound sentence variables. On a successful match,
//continues to the next required form. If all required forms have a corresponding
//antecedent, returns true iff there are no unused antecedents.
bool InferenceRule::findAntecedentsForForms(tree_list::iterator form, 
  ant_list& ant, bind_map& binds, statement_usage_map& ant_usage)
{
  //At the end, check if any antecedents were unused.
  if(form == required_forms.end()) return checkAntecedentRelevance(ant_usage);
  
  //The copy of the variable binding map is so new bindings can be removed if
  //a branch doesn't work out.
  bind_map temp_binds(binds);
  tree_list::iterator next_form = form;
  next_form++;
  
  //Try each antecedent for this form.
  for(ant_list::iterator itr = ant.begin(); itr != ant.end(); itr++)
  {
    if(*itr == NULL) return false;
    StatementTree* ant_data = (*itr)->getStatementData();
    if(ant_data == NULL) return false;
    
    //Attempt to match
    bool result = match(ant_data, *form, temp_binds);
    if(result)
    {
      //If successful, move on to the next required form.
      ant_usage[*itr]++;
      result = findAntecedentsForForms(next_form, ant, temp_binds, ant_usage);
      ant_usage[*itr]--;
    }
    //result = result && findAntecedentsForForms(next_form, ant, temp_binds);
    removeNewlyBoundForms(temp_binds, binds);
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

