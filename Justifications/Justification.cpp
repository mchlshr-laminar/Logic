#include "Justification.hpp"

using std::list;

Justification::Justification(ProofStatement* con) : consequent(con)
{}

//Adds the given statement to the list of antecedents if it was not already
//there, removes it if it was. Returns true if it was removed.
bool Justification::toggleAntecedent(ProofStatement* ant)
{
  if(ant == NULL) return false;
  for(list<ProofStatement*>::iterator itr = antecedents.begin(); itr != antecedents.end(); itr++)
  {
    if(*itr == ant) //Comparing addresses
    {
      antecedents.erase(itr);
      return true;
    }
  }
  antecedents.push_back(itr);
  return false;
}

//SUBPROOF PREMISE
bool SubproofPremise::toggleAntecedent(ProofStatemet* ant)
{ return false; }

bool SubproofPremise::isJustified()
{ return true; }

