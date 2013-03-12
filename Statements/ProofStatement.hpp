#ifndef __PROOF_STATEMENT_H_
#define __PROOF_STATEMENT_H_

#include <list>

class ProofStatement;
typedef std::list<ProofStatement*> ant_list;

#include "StatementTree.hpp"
#include "../Justifications/Justification.hpp"

class ProofStatement
{
  protected:
  ProofStatement* parent;
  int line_index;
  
  Justification* reason;
  StatementTree* data;
  ant_list antecedents;
  
  bool antecedentsAllowable();
  
  public:
  ProofStatement(const char* input);
  ProofStatement(StatementTree* input);
  
  virtual StatementTree* getStatementData();
  virtual StatementTree* getAssumption();
  virtual bool containsResult(StatementTree* match);
  virtual bool isJustified();
  
  ProofStatement* getParent();
  int getLineIndex();
  
  void rewrite(const char* input);
  void rewrite(StatementTree* input);
  void setJustification(Justification* new_reason);
  virtual bool toggleAntecedent(ProofStatement* ant);
};

#endif
