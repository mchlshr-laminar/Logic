#ifndef __SUBPROOF_H_
#define __SUBPROOF_H_

class SubProof;

#include "ProofStatement.hpp"
#include "../Justifications/Justification.hpp"

//Represents a subproof. Is a subclass of ProofStatement so that
//antecedents can be statements or subproofs.
class SubProof : public ProofStatement
{
  private:
  static Assumption subproof_assumption;
  statement_set contents;
  ProofStatement* assumption;
  
  public:
  SubProof(const char* input);
  SubProof(StatementTree* input);
  virtual ~SubProof();
  
  StatementTree* getStatementData();
  StatementTree* getAssumption();
  ProofStatement* getAssumptionStatement();
  bool isJustified();
  
  statement_set* getSubproofContents();
  int getLineIndex();
  char* createDisplayString();
  
  void rewrite(const char* input);
  void rewrite(StatementTree* input);
  
  bool toggleAntecedent(ProofStatement* ant);
  bool toggleChild(ProofStatement* ch);
};

#endif

