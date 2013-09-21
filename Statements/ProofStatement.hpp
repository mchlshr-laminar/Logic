#ifndef __PROOF_STATEMENT_H_
#define __PROOF_STATEMENT_H_

#include <list>
#include <set>

class ProofStatement;
typedef std::list<ProofStatement*> ant_list;
typedef std::set<ProofStatement*> statement_set;

#include "StatementTree.hpp"
#include "../Justifications/Justification.hpp"

//Represents one statement in the proof
class ProofStatement
{
  public:
  /*const static int NO_FAILURE = 0, INVALID_STATEMENT = 1, NO_JUSTIFICATION = 2,
    JUSTIFICATION_FAILURE = 3;*/
  enum failure_type_t { NO_FAILURE, INVALID_STATEMENT, NO_JUSTIFICATION,
    JUSTIFICATION_FAILURE};
  protected:
  ProofStatement* parent;
  int line_index;
  
  Justification* reason;
  StatementTree* data;
  ant_list antecedents;
  bool is_assumption;
  failure_type_t fail_type;
  
  public:
  ProofStatement(const char* input, bool is_assump = false);
  ProofStatement(StatementTree* input, bool is_assump = false);
  virtual ~ProofStatement();
  
  virtual StatementTree* getStatementData();
  virtual StatementTree* getAssumption();
  virtual bool isJustified();
  failure_type_t getFailureType();
  
  Justification* getJustification();
  ProofStatement* getParent();
  virtual statement_set* getSubproofContents();
  virtual int getLineIndex();
  bool isAssumption();
  virtual char* createDisplayString();
  
  virtual void rewrite(const char* input);
  virtual void rewrite(StatementTree* input);
  void setJustification(Justification* new_reason);
  virtual bool toggleAntecedent(ProofStatement* ant);
  void setParent(ProofStatement* new_parent);
  void setLineIndex(int i);
  
  protected:
  virtual bool toggleChild(ProofStatement* ch);
  ProofStatement* getRelevantAncestor(ProofStatement* antecedent);
  bool antecedentAllowable(ProofStatement* antecedent);
};

#endif
