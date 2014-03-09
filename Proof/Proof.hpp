#ifndef __PROOF_H_
#define __PROOF_H_

#include "../Statements/ProofStatement.hpp"
#include "../Statements/SubProof.hpp"
#include "../Statements/StatementTree.hpp"
#include "../Justifications/Justification.hpp"
#include "../Justifications/InferenceRules.hpp"
#include "../Justifications/EquivalenceRules.hpp"
#include "../Justifications/AggregateJustification.hpp"
#include <cstring>
#include <map>
#include <vector>
#include <string>

typedef std::vector<ProofStatement*> proof_list;

//Stores a proof. Proof is edited in a state-machine-like way; one line of the proof is currently focused
//and command execution is relative to that line (i.e. addLine adds a line directly after the focus etc.).
class Proof
{
  private:
  int current_position;
  int last_premise;
  proof_list proof_data;
  Assumption premise_just;
  StatementTree* goal;
  
  public:
  Proof();
  ~Proof();
  
  void setPosition(int new_position);
  void setStatement(const char* statement_string);
  void setGoal(const char* goal_string);
  
  void addLine();
  void setJustification(const char* justification_name);
  void toggleAntecedent(int antecedent_index);
  
  void addPremiseLine();
  
  void addSubproofLine();
  void endSubproof();
  
  void removeLine();
  
  //MOVE/REMOVE THESE
  bool addEquivalenceRule(const char* form1, const char* form2, const char* name);
  bool addInferenceRule(const std::vector<char*>& antecedents, const char* goal, const char* name);
  
  //A proof that ends with a subproof will cause verification to fail due to the empty statement after it.
  //I'm not sure why anyone would end a proof with a subproof, but this is not ideal.
  bool verifyProof();
  void printProof();
  char* createGoalString();
  void createPremiseStrings(std::vector<char*>& premise_strings);
  
  private:
  void printProofLine(int index);
};

#endif

