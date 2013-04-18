#include "SubProof.hpp"

StatementTree* SubProof::getStatementData()
{ return NULL; }

StatementTree* SubProof::getAssumption()
{ return data; }

bool SubProof::containsResult(StatementTree* match)
{ return false; }//Temp, though this function may not be needed.

bool SubProof::isJustified()
{ return data->isValid(); }

bool SubProof::toggleAntecedent(ProofStatement* ant)
{ return false; }

