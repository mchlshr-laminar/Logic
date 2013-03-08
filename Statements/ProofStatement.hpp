#ifndef __PROOF_STATEMENT_H_
#define __PROOF_STATEMENT_H_

#include "StatementTree.hpp"
#include "../Justifications/Justification.hpp"

class ProofStatement
{
  private:
  Justification reason;
  StatementTree data;
  
  public:
  ProofStatement(const char* input);
  ProofStatement(StatementTree* input);
  
  StatementTree* getData();
  bool isJustified();
};

#endif
