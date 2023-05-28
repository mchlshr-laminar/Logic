#include "SubProof.hpp"
#include <utility>

using std::pair;

Assumption SubProof::subproof_assumption;
//Static justification for all subproof assumptions.

SubProof::SubProof(const char* input) : ProofStatement(input)
{
  assumption = new ProofStatement(input, true);
  assumption->setParent(this);
  assumption->setJustification(&subproof_assumption);
}

SubProof::SubProof(StatementTree* input) : ProofStatement(input)
{
  assumption = new ProofStatement(input, true);
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

//Subproofs are justified iff their assumption is well-formed
bool SubProof::isJustified()
{
  bool result = assumption->getStatementData()->isValid();
  fail_type = (result)?NO_FAILURE:INVALID_STATEMENT;
  return result;
}

//Returns the set of statements in the subproof, including the
//assumption
statement_set* SubProof::getSubproofContents()
{ return &contents; }

int SubProof::getLineIndex()
{ return assumption->getLineIndex(); }

char* SubProof::createDisplayString()
{ return assumption->createDisplayString(); }

//Rewriting a subproof means rewriting its assumption
void SubProof::rewrite(const char* input)
{ assumption->rewrite(input); }

void SubProof::rewrite(StatementTree* input)
{ assumption->rewrite(input); }

bool SubProof::toggleAntecedent(ProofStatement* ant)
{ return false; }

bool SubProof::toggleChild(ProofStatement* childStatement)
{
  pair<statement_set::iterator, bool> result = contents.insert(childStatement);
  if(result.second) return false;
  contents.erase(result.first);
  return true;
}

