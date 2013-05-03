#include "Proof.hpp"
#include "../Statements/StatementTree.hpp"
#include <iostream>
#include <stack>
#include <utility>

using std::cout;
using std::cerr;
using std::endl;
using std::stack;
using std::pair;
using std::string;

Proof::Proof() : current_position(-1), last_premise(-1), goal(NULL)
{
  createJustifications();
}

Proof::~Proof()
{
  for(unsigned int i = 0; i < proof_data.size(); i++)
    delete proof_data[i];
  for(justification_map::iterator itr = rules.begin(); itr != rules.end(); itr++)
    delete itr->second;
  if(goal != NULL) delete goal;
}

void Proof::setPosition(int new_position)
{
  if(new_position < -1)
    current_position = -1;
  else if(new_position >= (int)(proof_data.size()))
    current_position = proof_data.size()-1;
  else
    current_position = new_position;
}

void Proof::setStatement(const char* statement_string)
{
  if(current_position == -1) return; //Shouldn't ever be >= size.
  
  proof_data[current_position]->rewrite(statement_string);
}

void Proof::setGoal(const char* goal_string)
{
  if(goal != NULL) delete goal;
  goal = new StatementTree(goal_string);
}

void Proof::addLine()
{
  if(current_position < last_premise) current_position = last_premise;
  //Premises before derivation
  
  ProofStatement* new_statement = new ProofStatement("");
  if(current_position >= 0 && current_position < (int)proof_data.size())
    new_statement->setParent(proof_data[current_position]->getParent());
  
  proof_list::iterator ins_pos = proof_data.begin()+current_position+1;
  proof_data.insert(ins_pos, new_statement);
  current_position++;
}

void Proof::setJustification(const char* justification_name)
{
  if(current_position <= last_premise) return;
  
  justification_map::iterator pos = rules.find(string(justification_name));
  if(pos != rules.end())
    proof_data[current_position]->setJustification(pos->second);
}

void Proof::toggleAntecedent(int antecedent_index)
{
  if(antecedent_index <= 0 || antecedent_index > (int)proof_data.size()) return;
  if(current_position <= last_premise || antecedent_index > current_position)
    return;
  
  proof_data[current_position]->toggleAntecedent(proof_data[antecedent_index-1]);
  StatementTree* t = proof_data[antecedent_index-1]->getStatementData();
  if(t == NULL) t = proof_data[antecedent_index-1]->getAssumption();
}

void Proof::addPremiseLine()
{
  if(current_position > last_premise) current_position = last_premise;
  //Premises before derivation.
  
  ProofStatement* new_premise = new ProofStatement("");
  new_premise->setJustification(&premise_just);
  
  proof_list::iterator ins_pos = proof_data.begin()+1+current_position;
  proof_data.insert(ins_pos, new_premise);
  current_position++;
  last_premise++;
}

void Proof::addSubproofLine()
{
  if(current_position < last_premise) current_position = last_premise;
  //Premises before derivation
  
  SubProof* new_proof = new SubProof("");
  
  if(current_position >= 0 && current_position < (int)proof_data.size())
  {
    new_proof->setParent(proof_data[current_position]->getParent());
    char* test =  proof_data[current_position]->getStatementData()->createDisplayString();
    bool is_empty = strcmp(test, "") == 0;
    delete [] test;
    if(is_empty)
      removeLine();
  }
  
  proof_list::iterator ins_pos = proof_data.begin()+current_position+1;
  proof_data.insert(ins_pos, new_proof->getAssumptionStatement());
  current_position++;
}

void Proof::endSubproof()
{
  if(current_position == -1 || proof_data[current_position]->getParent() == NULL)
  {
    addLine();
    return;
  }
  
  addLine();
  ProofStatement* new_parent = proof_data[current_position]->getParent();
  new_parent = new_parent->getParent();
  proof_data[current_position]->setParent(new_parent);
}

//MEMORY LEAKS
void Proof::removeLine()
{
  if(current_position == -1) return;
  if(current_position <= last_premise) last_premise--;
  
  proof_list::iterator pos = proof_data.begin()+current_position;
  if((*pos)->isAssumption())
  {
    proof_list::iterator epos = pos;
    for(; epos != proof_data.end(), (*epos)->getParent() == (*pos)->getParent(); epos++);
    proof_data.erase(pos, epos);
  }
  else
    proof_data.erase(pos);
  current_position--;
}

bool Proof::verifyProof()
{
  bool failed = false;
  int goal_index = (goal==NULL)?0:-1;
  for(unsigned int i = 0; i < proof_data.size(); i++)
  {
    if(!proof_data[i]->isJustified())
    {
      cout << "Line " << (i+1) << " is not justified: ";
      switch(proof_data[i]->getFailureType())
      {
        case ProofStatement::INVALID_STATEMENT:
          cout << "statement is invalid\n";
          break;
        case ProofStatement::NO_JUSTIFICATION:
          cout << "no inference/equivalence rule specified\n";
          break;
        case ProofStatement::JUSTIFICATION_FAILURE:
          cout << "rule could not be applied\n";
          break;
        default: cout << "unspecified failure\n";
          break;
      }
      
      failed = true;
    }
    if(goal_index == -1 && proof_data[i]->getParent()==NULL &&
      proof_data[i]->getStatementData()->equals(*goal))
    {
      goal_index = i;
    }
  }
  if(!failed) cout << "All lines check out\n";
  if(goal != NULL)
  {
    if(goal_index == -1) cout << "Goal not found\n";
    else cout << "Goal found at line " << (goal_index+1) << "\n";
  }
  return !failed;
}

void Proof::printProof()
{
  for(int i = 0; i < (int)proof_data.size(); i++)
    proof_data[i]->setLineIndex(i);
  
  int index = 0;
  stack<ProofStatement*> subproofs;
  for(; index <= last_premise; index++)
    printProofLine(index);
  cout << "   ]---\n";
  
  for(; index < (int)proof_data.size(); index++)
    printProofLine(index);
    
  if(goal != NULL)
  {
    char* goal_disp = goal->createDisplayString();
    cout << "Goal: " << goal_disp << "\n";
    delete [] goal_disp;
  }
}

void Proof::printProofLine(int index)
{
  int depth = 0; //Is inefficient, change later
  ProofStatement* traveller = proof_data[index]->getParent();
  for(; traveller != NULL; depth++, traveller = traveller->getParent());
  
  if(proof_data[index]->isAssumption() && index > last_premise)
  {
    cout << "   ";
    for(int i = 0; i <= depth-1; i++)
      cout << ']';
    cout << "\n";
  }
  
  
  cout << (index+1) << ((index < 99)?((index < 9)?"  ":" "):"");
  for(int i = 0; i <= depth; i++)
    cout << ']';
    
  char* display_string = proof_data[index]->createDisplayString();
  cout << " " << display_string << "\n";
  
  if(proof_data[index]->isAssumption() && index > last_premise)
  {
    cout << "   ";
    for(int i = 0; i <= depth; i++)
      cout << ']';
    cout << "---\n";
  }
}

void Proof::createJustifications()
{
  EquivalenceRule* rule;
  //EQUIVALENCE RULES
  rule = new EquivalenceRule("Association");
  rule->addEquivalentPair("a&(b&c)", "(a&b)&c");
  rule->addEquivalentPair("a|(b|c)", "(a|b)|c");
  rules[string("Association")] = rule;
  
  rule = new EquivalenceRule("Commutation");
  rule->addEquivalentPair("a&b", "b&a");
  rule->addEquivalentPair("a|b", "b|a");
  rules[string("Commutation")] = rule;
  
  rule = new EquivalenceRule("DeMorgan");
  rule->addEquivalentPair("!(a&b)", "!a|!b");
  //Consolidation of negation removes need for second version.
  rules[string("DeMorgan")] = rule;
  
  rule = new EquivalenceRule("Distribution");
  rule->addEquivalentPair("a&(b|c)", "(a&b)|(a&c)");
  rule->addEquivalentPair("a|(b&c)", "(a|b)&(a|c)");
  rules[string("Distribution")] = rule;
  
  rule = new EquivalenceRule("Idempotence");
  rule->addEquivalentPair("a&a", "a");
  rule->addEquivalentPair("a|a", "a");
  rules[string("Idempotence")] = rule;
  
  rule = new EquivalenceRule("Absorption");
  rule->addEquivalentPair("a&(a|b)", "a");
  rule->addEquivalentPair("a|(a&b)", "a");
  rules[string("Absorption")] = rule;
  
  rule = new EquivalenceRule("Reduction");
  rule->addEquivalentPair("a&(!a|b)", "a&b");
  rule->addEquivalentPair("a|(!a&b)", "a|b");
  rules[string("Reduction")] = rule;
  
  rule = new EquivalenceRule("Adjacency");
  rule->addEquivalentPair("(a|b)&(a|!b)", "a");
  rule->addEquivalentPair("(a&b)|(a&!b)", "a");
  rules[string("Adjacency")] = rule;
  
  //CONDITIONAL EQUIVALENCE RULES
  rule = new EquivalenceRule("Implication");
  rule->addEquivalentPair("a>b", "!a|b");
  //Second version is first + DeMorgan.
  rules[string("Implication")] = rule;
  
  rule = new EquivalenceRule("Contraposition");
  rule->addEquivalentPair("a>b", "!b>!a");
  rules[string("Contraposition")] = rule;
  
  rule = new EquivalenceRule("Exportation");
  rule->addEquivalentPair("a>(b>c)", "(a&b)>c");
  rules[string("Exportation")] = rule;
  
  rule = new EquivalenceRule("Equivalence");
  rule->addEquivalentPair("a=b", "(a>b)&(b>a)");
  rule->addEquivalentPair("a=b", "(a&b)|(!a&!b)");
  rules[string("Equivalence")] = rule;
  
  rule = new EquivalenceRule("Conditional Distribution");
  rule->addEquivalentPair("a>(b&c)", "(a>b)&(a>c)");
  rule->addEquivalentPair("a>(b|c)", "(a>b)|(a>c)");
  rule->addEquivalentPair("(a|b)>c", "(a>c)&(b>c)");
  rule->addEquivalentPair("(a&b)>c", "(a>c)|(b>c)");
  rules[string("Conditional Distribution")] = rule;
  
  rule = new EquivalenceRule("Conditional Reduction");
  rule->addEquivalentPair("(a>b)&a", "a&b");
  rule->addEquivalentPair("(a>b)&!b", "!a&!b");
  rules[string("Conditional Reduction")] = rule;
  //Conditional Idempotence by implication & idempotence
  
  //INFERENCE RULES
  InferenceRule* temp;
  AggregateJustification* ag;
  //and
  temp = new InferenceRule("a&b", "Conjunction");
  temp->addRequiredForm("a");
  temp->addRequiredForm("b");
  rules[string("Conjunction")] = temp;
  
  ag = new AggregateJustification("Simplification");
  temp = new InferenceRule("a", "simp1");
  temp->addRequiredForm("a&b");
  ag->addRule(temp);
  temp = new InferenceRule("b", "simp2");
  temp->addRequiredForm("a&b");
  ag->addRule(temp);
  rules[string("Simplification")] = ag;
  
  //or
  ag = new AggregateJustification("Addition");
  temp = new InferenceRule("a|b", "add1");
  temp->addRequiredForm("a");
  ag->addRule(temp);
  temp = new InferenceRule("a|b", "add2");
  temp->addRequiredForm("b");
  ag->addRule(temp);
  rules[string("Addition")] = ag;
  
  temp = new InferenceRule("c", "Proof by Cases");
  temp->addRequiredForm("a|b");
  temp->addRequiredForm("c", "a");
  temp->addRequiredForm("c", "b");
  rules[string("Proof by Cases")] = temp;
  
  //not
  temp = new InferenceRule("a", "Indirect Proof");
  temp->addRequiredForm("b&!b", "!a");
  rules[string("Indirect Proof")] = temp;
  
  //Not elimination is moot
  
  //if
  temp = new InferenceRule("a>b", "Conditional Proof");
  temp->addRequiredForm("b", "a");
  rules[string("Conditional Proof")] = temp;
  
  temp = new InferenceRule("b", "Modus Ponens");
  temp->addRequiredForm("a>b");
  temp->addRequiredForm("a");
  rules[string("Modus Ponens")] = temp;
  
  //iff
  temp = new InferenceRule("a=b", "Biconditional Proof");
  temp->addRequiredForm("a", "b");
  temp->addRequiredForm("b", "a");
  rules[string("Biconditional Proof")] = temp;
  
  ag = new AggregateJustification("Biconditional Elimination");
  temp = new InferenceRule("a", "be1");
  temp->addRequiredForm("a=b");
  temp->addRequiredForm("b");
  ag->addRule(temp);
  temp = new InferenceRule("b", "be2");
  temp->addRequiredForm("a=b");
  temp->addRequiredForm("a");
  ag->addRule(temp);
  rules[string("Biconditional Elimination")] = ag;
  
  //other
  temp = new InferenceRule("!a", "Modus Tollens");
  temp->addRequiredForm("a>b");
  temp->addRequiredForm("!b");
  rules[string("Modus Tollens")] = temp;
  //Disjunctive syllogism using implication
  
  temp = new InferenceRule("a>c", "Hypothetical Syllogism");
  temp->addRequiredForm("a>b");
  temp->addRequiredForm("b>c");
  rules[string("Hypothetical Syllogism")] = temp;
  
  temp = new InferenceRule("a", "Reiteration");
  temp->addRequiredForm("a");
  rules[string("Reiteration")] = temp;
  
  temp = new InferenceRule("a|!a", "Excluded Middle");
  rules[string("Excluded Middle")] = temp;
}

