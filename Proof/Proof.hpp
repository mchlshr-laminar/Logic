#ifndef __PROOF_H_
#define __PROOF_H_

#include "../Statements/ProofStatement.hpp"
#include "../Statements/SubProof.hpp"
#include "../Justifications/Justification.hpp"
#include "../Justifications/InferenceRules.hpp"
#include "../Justifications/EquivalenceRules.hpp"
#include <cstring>
#include <map>
#include <vector>

class Proof;

class StringMapper
{
  bool operator()(char* str1, char* str2)
  { return strcmp(str1,str2) < 0; }
};

typedef std::vector<ProofStatement*> proof_list;
typedef std::map<char*, Justification*, StringMapper> justification_map;

class Proof
{
  private:
  int current_position;
  int last_premise;
  proof_list proof_data;
  justification_map rules;
  Assumption premise_just;
  
  public:
  Proof();
  ~Proof();
  
  void setPosition(int new_position);
  void setStatement(const char* statement_string);
  
  void addLine();
  void setJustification(const char* justification_name);
  void toggleAntecedent(int antecedent_index);
  
  void addPremiseLine();
  
  void addSubproofLine();
  void endSubproof();
};

#endif

