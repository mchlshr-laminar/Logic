#ifndef __JUSTIFICATION_H_
#define __JUSTIFICATION_H_

#include "../Statements/StatementTree.hpp"
#include "../Statements/ProofStatement.hpp"
#include <list>

class Justification
{
  protected:
  std::list<ProofStatement*> antecedents;
  ProofStatement* consequent;
  
  public:
  Justification(StatementTree* con);
  virtual bool toggleAntecedent(StatementTree* ant);
  
  virtual bool isJustified() = 0;
};

class SubproofPremise : public Justification
{
  public:
  SubproofPremise(ProofStatement* con) : Justification(con)
  {}
  
  bool toggleAntecedent(StatementTree* ant);
  bool isJustified();
};

class InferenceRule : public Justification
{
  public:
  InferenceRule(ProofStatement* con) : Justification(con)
  {}
  
  bool isJustified();
};

#endif