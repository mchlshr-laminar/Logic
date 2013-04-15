#ifndef __SUBPROOF_H_
#define __SUBPROOF_H_

class SubProof;

#include "ProofStatement.hpp";
#include "../Justifications/Justification.hpp"

class SubProof : public ProofStatement
{
  private:
  
  public:
  StatementTree* getStatementData();
  StatementTree* getAssumption();
  bool containsResult(StatementTree* match);
  //Maybe have antecedent be specific result, justification checks antecedent's
  //parent's assumptions?
  bool isJustified();
  
  bool toggleAntecedent(ProofStatement* ant);
};

#endif

