#ifndef __SUBPROOF_H_
#define __SUBPROOF_H_

class SubProof;

#include "ProofStatement.hpp"
#include "../Justifications/Justification.hpp"

class SubProof : public ProofStatement
{
  private:
  static Assumption subproof_assumption;
  statement_set contents;
  ProofStatement* assumption;
  
  public:
  SubProof(const char* input);
  SubProof(StatementTree* input);
  ~SubProof();
  
  StatementTree* getStatementData();
  StatementTree* getAssumption();
  ProofStatement* getAssumptionStatement();
  bool containsResult(StatementTree* match);
  //Maybe have antecedent be specific result, justification checks antecedent's
  //parent's assumptions?
  bool isJustified();
  
  statement_set* getSubproofContents();
  
  void rewrite(const char* input);
  void rewrite(StatementTree* input);
  
  bool toggleAntecedent(ProofStatement* ant);
  bool toggleChild(ProofStatement* ch);
};

#endif

