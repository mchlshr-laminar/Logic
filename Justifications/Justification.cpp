#include "Justification.hpp"

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

char* Justification::getName()
{ return rule_name; }

