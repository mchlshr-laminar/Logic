#ifndef __PROOF_H_
#define __PROOF_H_

#include "../Statements/ProofStatement.hpp"
#include "../Statements/SubProof.hpp"
#include "../Justifications/Justification.hpp"
#include "../Justifications/InferenceRules.hpp"
#include "../Justifications/EquivalenceRules.hpp"
#include <cstring>
#include <vector>
#include <map>

class Proof;

typedef std::vector<ProofStatement*> proof_list;
typedef std::map<char*, Justification*> justification_map;

class Proof
{
  private:
  justification_map rules;
  proof_list statements;
  int last_premise;
  int current_position;
  int insert_position;
  ProofStatement* current_subproof;
  Assumption premise_just;
  
  public:
  Proof();
  ~Proof();
  
  void addStatement(const char* statement_string);
  void setJustification(const char* justification_name);
  void toggleAntecedent(ProofStatement* antecedent);
  void toggleAntecedent(int antecedent_index);
  
  void addPremise(const char* premise_string);
  
  void addSubproof(const char* assumption_string);
  void exitSubproof();
  
  private:
  int wInc(int index);
}

#endif

