#include "Justification.hpp"
#include <cstring>

//Stores the name of this rule.
Justification::Justification(const char* name)
{
  if(name == NULL)
  {
    rule_name = NULL;
    return;
  }
  rule_name = new char[strlen(name)+1];
  strcpy(rule_name, name);
}

Justification::~Justification()
{
  if(rule_name != NULL)
  {
    delete [] rule_name;
    rule_name = NULL;
  }
}

//Returns the name of this rule.
char* Justification::getName()
{ return rule_name; }

//Deallocates the values in the given map & clears the map.
void Justification::removeBoundForms(bind_map& binds)
{
  for(bind_map::iterator itr = binds.begin(); itr != binds.end(); itr++)
    delete itr->second;
  binds.clear();
}

//Deallocates values & removes entries in new_binds for which the key value does
//not appear in old_binds.
void Justification::removeNewlyBoundForms(bind_map& new_binds, bind_map& old_binds)
{
  for(bind_map::iterator itr = new_binds.begin(); itr != new_binds.end(); itr++)
  {
    if(old_binds.find(itr->first) == old_binds.end())
    {
      delete itr->second;
      bind_map::iterator temp = itr;
      itr++;
      new_binds.erase(temp);
    }
  }
}

