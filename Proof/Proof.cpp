#include "Proof.hpp"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

Proof::Proof() : last_premise(-1), current_position(-1),
  insert_position(0), current_subproof(NULL)
{
}

Proof::~Proof()
{
  for(justification_map::iterator itr = rules.begin(); itr != rules.end(); itr++)
  {
    delete [] itr->first;
    delete itr->second;
  }
  for(proof_list::iterator itr = statements.begin(); itr != statements.end(); itr++)
    delete *itr;
}

int Proof::wInc(int index)
{ return (index <= statements.size())?(index + 1):0; }

void Proof::addStatement(const char* statement_string)
{
  //all statements go after the premises.
  if(insert_position <= last_premise)
  {
    current_position = last_premise;
    insert_position = wInc(current_position);
  }
  proof_list::iterator ins_itr = statements.begin()+insert_position;
  ProofStatement* new_statement = new ProofStatement(statement_string);
  new_statement->setParent(current_subproof);
  statements.insert(ins_itr, new_statement);
  
  current_position = insert_position; //There may be subproofs
  insert_position++;
}

void Proof::setJustification(const char* justification_name)
{
  if(current_position <= last_premise) return;
  
  justification_map::iterator position = rules.find(justification_name);
  if(position != rules.end())
    statements[current_position]->setJustification(*position);
}

void Proof::toggleAntecedent(ProofStatement* antecedent)
{
  if(current_position <= last_premise) return;
  
  //Check position in proof?
  statements[current_position]->toggleAntecedent(antecedent);
}

void Proof::toggleAntecedent(int antecedent_index)
{
  if(current_position <= last_premise) return;
  if(antecedent_index >= current_position) return;
  if(antecedent_index < 0 || antecedent_index >= statements.size()) return;
  
  statements[current_position]->toggleAntecedent(statements[antecedent_index]);
}

void Proof::addPremise(const char* premise_string)
{
  ProofStatement* new_premise = new ProofStatement(premise_string);
  new_premise->setJustification(&premise_just);
  statements.push_front(new_premise);
  last_premise++;
  current_position++;
  insert_position++;
}

void Proof::addSubproof(const char* assumption_string)
{
  SubProof* new_subproof = new SubProof(assumption_string);
  new_subproof->setParent(current_subproof);
  
  current_position = insert_position;
  insert_position++;
  current_subproof = new_subproof;
}

void Proof::exitSubproof()
{
  //DO STUFF
}

