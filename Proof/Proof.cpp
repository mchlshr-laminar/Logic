#include "Proof.hpp"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

Proof::Proof() : current_position(-1), last_premise(-1)
{
  //ADD JUSTIFICATIONS
}

Proof::~Proof()
{
  for(int i = 0; i < proof_data.size(); i++)
    delete proof_data[i];
  for(justification_map::iterator itr = rules.begin(); itr != rules.end(); itr++)
  {
    delete [] itr->first;
    delete itr->second;
  }
}

void Proof::setPosition(int new_position)
{
  if(new_position >= -1 && new_position < proof_data.size())
    current_position = new_position;
}

void Proof::setStatement(const char* statement_string);
{
  if(current_position == -1) return; //Shouldn't ever be >= size.
  
  proof_data[current_position]->rewrite(statement_string);
}

void Proof::addLine()
{
  if(current_position < last_premise) current_position = last_premise;
  //Premises before derivation
  
  ProofStatement* new_statement = new ProofStatement("");
  new_statement->setParent(proof_data[current_position]->getParent());
  
  proof_list::iterator ins_pos = proof_data.begin()+current_position+1;
  proof_data.insert(ins_pos, new_statement);
  current_position++;
}

void Proof::setJustification(const char* justification_name)
{
  if(current_position <= last_premise) return;
  
  justification_map::iterator pos = rules.find(justification_name);
  if(pos != rules.end())
    proof_data[current_position]->setJustification(*itr);
}

void Proof::toggleAntecedent(int antecedent_index)
{
  if(antecedent_index < 0 || antecedent_index >= proof_data.size()) return;
  if(current_position <= last_premise || antecedent_index <= current_position)
    return;
  
  proof_data[current_position]->toggleAntecedent(proof_data[antecedent_index]);
}

void Proof::addPremiseLine()
{
  if(current_position > last_premise) current_position = last_premise;
  //Premises before derivation.
  
  ProofStatement* new_premise = new ProofStatement("");
  new_statement->setJustification(&premise_just);
  
  proof_list::iterator ins_pos = proof_data.begin()+current_position+1;
  proof_data.insert(ins_pos, new_premise);
  current_position++;
  last_premise++;
}

void Proof::addSubproofLine()
{
  if(current_position < last_premise) current_position = last_premise;
  //Premises before derivation
  
  SubProof* new_proof = new SubProof("");
  new_proof->setParent(proof_data[current_position]->getParent());
  
  proof_list::iterator ins_pos = proof_data.begin()+current_position+1;
  proof_data.insert(ins_pos, new_statement);
  current_position++;
}

void Proof::endSubproof()
{
  if(current_position == -1 || proof_data[current_position]->getParent() == NULL)
    return;
  
  addLine();
  ProofStatement* new_parent = proof_data[current_position]->getParent();
  new_parent = new_parent->getParent();
  proof_data[current_position]->setParent(new_parent);
}

