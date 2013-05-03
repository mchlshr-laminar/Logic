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

class Proof;

class StringMapper
{
  public:
  bool operator()(std::string str1, std::string str2)
  { return str1.compare(str2) < 0; }
};

typedef std::vector<ProofStatement*> proof_list;
typedef std::map<std::string, Justification*, StringMapper> justification_map;

class Proof
{
  private:
  int current_position;
  int last_premise;
  proof_list proof_data;
  justification_map rules;
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
  
  bool verifyProof();
  void printProof();
  
  private:
  void printProofLine(int index);
  void createJustifications();
};

#endif

