#include "EquivalenceRules.hpp"
#include <list>
#include <utility>
#include <iostream>

using std::map;
using std::list;
using std::pair;
using std::cout;
using std::endl;

EquivalenceRule::~EquivalenceRule()
{
  list<equiv_pair>::iterator itr = equivalent_pairs.begin();
  for(; itr != equivalent_pairs.end(); itr++)
  {
    delete itr->first;
    delete itr->second;
  }
}

//Adds a pair of equivalent sentences which can be applied.
void EquivalenceRule::addEquivalentPair(const char* form1, const char* form2)
{
  equiv_pair new_equivalence(new StatementTree(form1), new StatementTree(form2));
  if(!new_equivalence.first->isValid() || !new_equivalence.second->isValid())
  {
    delete new_equivalence.first;
    delete new_equivalence.second;
    return;
  }
  equivalent_pairs.push_back(new_equivalence);
}

bool EquivalenceRule::isJustified(StatementTree& con, ant_list& ant)
{
  if(ant.size() != 1) return false;
  return areEquivalent(&con, ant.front()->getStatementData());
}

//Checks if the given sentences are equivalent using only the equivalences given to
//this rule.
bool EquivalenceRule::areEquivalent(StatementTree* tree1, StatementTree* tree2)
{
  if(tree1 == NULL || tree2 == NULL) return false;
  bind_map binds;
  
  //Root is the same, check equivalence of children
  if(tree1->nodeType() == tree2->nodeType() && tree1->isAffirmed() == tree2->isAffirmed())
  {
    if(tree1->nodeType() == StatementTree::ATOM) return tree1->equals(*tree2);
    //Atoms must be equal to be equivalent
    
    child_itr itr1 = tree1->begin();
    child_itr itr2 = tree2->begin();
    bool all_children_equiv = true; //Might be unneeded; itrs wont reach end if break happens
    for(; itr1 != tree1->end(), itr2 != tree2->end(); itr1++, itr2++)
    {
      if(!areEquivalent(*itr1, *itr2))
      {
        all_children_equiv = false;
        break;
      }
    }
    if(all_children_equiv && itr1 == tree1->end() && itr2 == tree2->end())
      return true;
    //It's possible equivalent pairs would have same root so don't return false
    //if this doesn't work.
    //Could just put this segment after equivalence pairs but that might be
    //slower with buried equivalence.
  }
  
  //Check the equivalent pairs
  list<equiv_pair>::iterator itr = equivalent_pairs.begin();
  for(; itr != equivalent_pairs.end(); itr++)
  {
    //tree1 is of first form & tree2 is of second
    matchFormOneNegation(tree1, *itr);
    bool result = match(tree1, itr->first, binds) && match(tree2, itr->second, binds);
    removeBoundForms(binds);
    if(result) return true;
    
    //tree1 is of second form & tree2 is of first
    matchFormOneNegation(tree2, *itr);
    result = match(tree1, itr->second, binds) && match(tree2, itr->first, binds);
    removeBoundForms(binds);
    if(result) return true;
  }
  
  //Nothing worked
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
    else
    {
      bool result = areEquivalent(sentence, retval.first->second);
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

//Simultaneously toggles form negation to ensure form 1's negation
//matches the target. (note a == !b <==> !a == b).
void EquivalenceRule::matchFormOneNegation(StatementTree* target, equiv_pair& source)
{
  if(target->isAffirmed() != source.first->isAffirmed())
  {
    source.first->negate();
    source.second->negate();
  }
}

