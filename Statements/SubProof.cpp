#include "SubProof.hpp"
#include <utility>

using std::pair;

StatementTree* SubProof::getStatementData()
{ return NULL; }

StatementTree* SubProof::getAssumption()
{ return data; }

bool SubProof::containsResult(StatementTree* match)
{ return false; }//Temp, though this function may not be needed.

bool SubProof::isJustified()
{ return data->isValid(); }

statement_set* SubProof::getSubproofContents()
{ return &contents; }

bool SubProof::toggleAntecedent(ProofStatement* ant)
{ return false; }

bool SubProof::toggleChild(ProofStatement* ch)
{
  pair<statement_set::iterator, bool> result = contents.insert(ch);
  if(result.second) return false;
  contents.erase(result.first);
  return true;
}

