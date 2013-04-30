#ifndef __PROOF_STATEMENT_H_
#define __PROOF_STATEMENT_H_

#include <list>
#include <set>

class ProofStatement;
typedef std::list<ProofStatement*> ant_list;
typedef std::set<ProofStatement*> statement_set;

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
  virtual statement_set* getSubproofContents();
  int getLineIndex();
  
  void rewrite(const char* input);
  void rewrite(StatementTree* input);
  void setJustification(Justification* new_reason);
  virtual bool toggleAntecedent(ProofStatement* ant);
  void setParent(ProofStatement* new_parent);
  
  protected:
  virtual bool toggleChild(ProofStatement* ch);
};

#endif
