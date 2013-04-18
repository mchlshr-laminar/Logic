#ifndef __SUBPROOF_H_
#define __SUBPROOF_H_

class SubProof;

#include "ProofStatement.hpp"
#include "../Justifications/Justification.hpp"

class SubProof : public ProofStatement
{
  private:
  statement_set contents;
  
  public:
  SubProof(const char* input) : ProofStatement(input)
  {}
  SubProof(StatementTree* input) : ProofStatement(input)
  {}
  
  StatementTree* getStatementData();
  StatementTree* getAssumption();
  bool containsResult(StatementTree* match);
  //Maybe have antecedent be specific result, justification checks antecedent's
  //parent's assumptions?
  bool isJustified();
  
  statement_set* getSubproofContents();
  
  bool toggleAntecedent(ProofStatement* ant);
  bool toggleChild(ProofStatement* ch);
};

#endif

