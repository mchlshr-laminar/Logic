#include "SubProof.hpp"
#include <utility>

using std::pair;

Assumption SubProof::subproof_assumption;

SubProof::SubProof(const char* input) : ProofStatement(input)
{
  assumption = new ProofStatement(input);
  assumption->setParent(this);
  assumption->setJustification(&subproof_assumption);
}

SubProof::SubProof(StatementTree* input) : ProofStatement(input)
{
  assumption = new ProofStatement(input);
  assumption->setParent(this);
  assumption->setJustification(&subproof_assumption);
}

SubProof::~SubProof()
{ delete assumption; }

StatementTree* SubProof::getStatementData()
{ return NULL; }

StatementTree* SubProof::getAssumption()
{ return assumption->getStatementData(); }

ProofStatement* SubProof::getAssumptionStatement()
{ return assumption; }

bool SubProof::containsResult(StatementTree* match)
{ return false; }//Temp, though this function may not be needed.

bool SubProof::isJustified()
{ return assumption->getStatementData()->isValid(); }

statement_set* SubProof::getSubproofContents()
{ return &contents; }

void SubProof::rewrite(const char* input)
{ assumption->rewrite(input); }

void SubProof::rewrite(StatementTree* input)
{ assumption->rewrite(input); }

bool SubProof::toggleAntecedent(ProofStatement* ant)
{ return false; }

bool SubProof::toggleChild(ProofStatement* ch)
{
  pair<statement_set::iterator, bool> result = contents.insert(ch);
  if(result.second) return false;
  contents.erase(result.first);
  return true;
}

