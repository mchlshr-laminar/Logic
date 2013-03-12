#include "EquivalenceRules.hpp"
#include <list>
#include <utility>
#include <iostream>

using std::map;
using std::list;
using std::pair;
using std::cout;
using std::endl;

bool EquivalenceRule::isJustified(StatementTree& con, ant_list& ant)
{
  if(ant.size() != 1) return false;
  return areEquivalent(con.getStatementData(), ant.front()->getStatementData());
}

//Returns whether or not the two given sentences are equivalent using only this
//equivalence rule.
bool EquivalenceRule::areEquivalent(StatementTree* tree1, StatementTree* tree2)
{
  if(tree1 == NULL || tree2 == NULL) return false;
  
  bind_map binds;
  
  //tree1 is of form 1 & tree2 is of form 2
  matchFormOneNegation(tree1);
  if(match(tree1, &form1, binds) && match(tree2, &form2, binds)) return true;
  binds.clear();
  //Deallocate bound sentences?
  
  //vice versa
  matchFormOneNegation(tree2);
  if(match(tree1, &form2, binds) && match(tree2, &form1, binds)) return true;
  binds.clear();
  
  //Root is the same type, check equivalence of children (or atom name if relevant)
  if(tree1->nodeType() == tree2->nodeType() && tree1->isAffirmed() == tree2->isAffirmed())
  {
    if(tree1->nodeType() == StatementTree::ATOM) return tree1->equals(*tree2);
    
    child_itr itr1 = tree1->begin();
    child_itr itr2 = tree2->begin();
    for(; itr1 != tree1->end(), itr2 != tree2->end(); itr1++, itr2++)
      if(!areEquivalent(*itr1, *itr2)) return false;
    return itr1 == tree1->end() && itr2 == tree2->end();
  }
  
  return false;
}

//Returns whether or not target can fit the given form while maintaining any previous
//sentence variable bindings.
bool EquivalenceRule::match(StatementTree* target, StatementTree* form, bind_map& binds)
{
  //The form is a sentence variable; if it's unbound, bind & return. Else return whether
  //the target is equivalent to the bound sentence.
  if(form->nodeType() == StatementTree::ATOM)
  {
    StatementTree* sentence = new StatementTree(*target, form->isAffirmed());
    pair<bind_map::iterator, bool> retval = 
      binds.insert(pair<char, StatementTree*>(form->atomName()[0], sentence));
    
    if(retval.second) return true;
    else return areEquivalent(sentence, retval.first->second);
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

//Simultaneously toggles form negation to ensure form 1's negation
//matches the target. (note a == !b <==> !a == b).
void EquivalenceRule::matchFormOneNegation(StatementTree* target)
{
  if(target->isAffirmed() != form1.isAffirmed())
  {
    form1.negate();
    form2.negate();
  }
}
