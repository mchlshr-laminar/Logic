#include "Proof.hpp"
#include "ProofRules.hpp"
#include "StatementTree.hpp"
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
  //This space left intentionally blank
}

Proof::~Proof()
{
  for(unsigned int i = 0; i < proof_data.size(); i++)
    delete proof_data[i];
  if(goal != NULL) delete goal;
}

//Sets the current focus position for editing.
void Proof::setPosition(int new_position)
{
  if(new_position < -1)
    current_position = -1; //Focus before the first line
  else if(new_position >= (int)(proof_data.size()))
    current_position = proof_data.size()-1; //Can't do focus after the last line
  else
    current_position = new_position;
}

//Sets the sentence of the focused statement to the given
//string.
void Proof::setStatement(const char* statement_string)
{
  if(current_position == -1) return; //Shouldn't ever be >= size.
  
  proof_data[current_position]->rewrite(statement_string);
}

//Sets the goal statement of this proof.
void Proof::setGoal(const char* goal_string)
{
  if(goal != NULL) delete goal;
  goal = new StatementTree(goal_string);
}

//Adds a new proof line after the focused one (or after the premises
//if focus is on a premise line) and sets focus to the new line. This
//line will be in the same subproof as the previously focused line.
void Proof::addLine()
{
  //Premises before derivation
  if(current_position < last_premise) current_position = last_premise;
  
  //Create the statement and put it in the correct subproof
  ProofStatement* new_statement = new ProofStatement("");
  if(current_position >= 0 && current_position < (int)proof_data.size())
    new_statement->setParent(proof_data[current_position]->getParent());
  
  //Insert into the proof
  proof_list::iterator ins_pos = getLineInsertionIterator();
  proof_data.insert(ins_pos, new_statement);
  current_position++;
}

//Adds a premise line after the focused line (or at the end of the premises
//if focus is not on a premise) and sets focus to the new line.
void Proof::addPremiseLine()
{
    //Premises before derivation.
	if (current_position > last_premise) current_position = last_premise;

    //Create the statement and apply the premise Assumption justification
	ProofStatement* new_premise = new ProofStatement("");
	new_premise->setJustification(&premise_just);

    //Insert into the proof, update the index to the last premise
	proof_list::iterator ins_pos = getLineInsertionIterator();
	proof_data.insert(ins_pos, new_premise);
	current_position++;
	last_premise++;
}

//Adds a new subproof assumption line (creating a new subproof). Will
//be placed after the currently focused line, or after the premises if
//focus is in the premises. Subproof's parent will be the same as the
//previously focused line. Sets focus to the new assumption line.
//If the focused line has an empty sentence, deletes the line & replaces
//it.
void Proof::addSubproofLine()
{
  //Premises before derivation
	if (current_position < last_premise) current_position = last_premise;

	SubProof* new_proof = new SubProof("");

    //Create the subproof and put it into a parent subproof (if relevant)
	if (current_position >= 0 && current_position < (int)proof_data.size())
	{
		new_proof->setParent(proof_data[current_position]->getParent());
		char* test = proof_data[current_position]->getStatementData()->createDisplayString();
		bool is_empty = strcmp(test, "") == 0;
		delete[] test;
		if (is_empty)
			removeLine();
	}

    //Insert the new subproof's assumption line into the proof
	proof_list::iterator ins_pos = getLineInsertionIterator();
	proof_data.insert(ins_pos, new_proof->getAssumptionStatement());
	current_position++;
}

//Adds a new proof line as per AddLine, except that it will be in the current
//subproof's parent.
void Proof::endSubproof()
{
    //TODO: check that we aren't in the middle of a subproof (rather than the end).
    addLine();
    if (current_position == -1 || proof_data[current_position]->getParent() == NULL)
      return; //Not in a subproof, just add the line

    //Move the new line out of the innermost subproof
	ProofStatement* new_parent = proof_data[current_position]->getParent();
	new_parent = new_parent->getParent();
	proof_data[current_position]->setParent(new_parent);
}

//Get an iterator to insert a line after the currently focused line.
proof_list::iterator Proof::getLineInsertionIterator()
{
	if (current_position >= (int)proof_data.size())
	{
		//current_position may be -1 and size() returns an unsigned value
		cerr << "Position is after the end of the proof\n";
		return proof_data.begin();
	}

	if (proof_data.size() == 0)
	{
        //Nothing in the proof, insert at the beginning
		return proof_data.begin();
	}
	return proof_data.begin() + current_position + 1;
}

//Searches for the justification rule with the given name. If found and the
//focused statement is not a premise/assumption, sets the statement's justification
//to the found rule.
void Proof::setJustification(const char* justification_name)
{
  if(current_position <= last_premise) return; //Don't change justification for premises
  
  Justification* justification_rule = ProofRules::findRule(justification_name);
  if(justification_rule != NULL)
    proof_data[current_position]->setJustification(justification_rule);
}

//Toggles whether or not the specified line is listed as an antecedent of the
//focused line. Does nothing if focus is an assumption or premise or the specified
//line is not before the focus.
void Proof::toggleAntecedent(int antecedent_index)
{
	if (antecedent_index < 0 || antecedent_index >= (int)proof_data.size()) return; //Not a valid index
	if (current_position <= last_premise || antecedent_index >= current_position) return; //Current line is a premise, or proposed antecedent is after current line
    if (proof_data[current_position]->isAssumption()) return; //Current line is a subproof assumption

	proof_data[current_position]->toggleAntecedent(proof_data[antecedent_index]);
}

//Removes the focused line and decrements the focus.
//TODO: Redo this
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

//The following two functions should be moved out of this class; they're left over from when the 
//justification_map was a member of Proof.
bool Proof::addEquivalenceRule(const char* form1, const char* form2, const char* name)
{
  if(ProofRules::findRule(name) != NULL) return false; //A rule by that name already exists
  
  EquivalenceRule* added_equivalence = new EquivalenceRule(name);
  added_equivalence->addEquivalentPair(form1, form2);
  
  ProofRules::addRule(added_equivalence);
  return true;
}

bool Proof::addInferenceRule(const std::vector<char*>& antecedents, const char* goal, const char* name)
{
  if(ProofRules::findRule(name) != NULL) return false; //A rule by that name already exists
  
  InferenceRule* added_inference = new InferenceRule(goal, name);
  for(unsigned int i = 0; i < antecedents.size(); i++)
    added_inference->addRequiredForm(antecedents[i]);
  
  ProofRules::addRule(added_inference);
  return true;
}

//Checks and prints if the proof works. Note again that a proof that ends in a subproof will fail.
bool Proof::verifyProof()
{
  bool failed = false;
  //int goal_index = (goal==NULL)?0:-1; //Only check that the goal was found if there is a goal
  int goal_index = -1;
  bool has_goal = goal != NULL;

  for(unsigned int i = 0; i < proof_data.size(); i++)
  {
    //Check each line
    if(!proof_data[i]->isJustified())
    {
      //Line is not justified, print the reason why
      cout << "Line " << (i+1) << " is not justified: ";
      switch(proof_data[i]->getFailureType())
      {
        case ProofStatement::INVALID_STATEMENT:
          cout << "sentence is not well-formed\n";
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
    if(has_goal && goal_index == -1 && proof_data[i]->getParent()==NULL &&
      proof_data[i]->getStatementData()->equals(*goal))
    {
      //The proof has a goal, and the current line matches it and is not in
      //a subproof, i.e. we've found the goal.
      //Note that we don't need to check for assumptions, as subproof
      //assumptions are ruled out by being in subproofs. Technically it's
      //valid for a premise to match the goal, even if it isn't much of a
      //proof.
      goal_index = i;
    }
  }

  //Print the results of verification
  if(!failed) cout << "All lines check out\n";
  if(has_goal)
  {
    if(goal_index == -1)
    {
      cout << "Goal not found\n";
      failed = true;
    }
    else cout << "Goal found at line " << (goal_index+1) << "\n";
  }
  return !failed;
}

//Displays the proof.
void Proof::printProof()
{
  //Tell each line what index should be displayed for each line
  for(int i = 0; i < (int)proof_data.size(); i++)
    proof_data[i]->setLineIndex(i);
  
  //Print premise lines, followed by a seperator
  int index = 0;
  for(; index <= last_premise; index++)
    printProofLine(index);
  cout << "   ]---\n";
  
  //Print the lines of the proof
  for(; index < (int)proof_data.size(); index++)
    printProofLine(index);
    
  //Print the goal
  if(goal != NULL)
  {
    char* goal_disp = goal->createDisplayString();
    cout << "Goal: " << goal_disp << "\n";
    delete [] goal_disp;
  }
}

void Proof::printProofLine(int index)
{
  //Determine how far to indent the line, i.e. how many subproofs it's in
  int depth = 0; //Is inefficient, change later (pass depth as parameter?)
  ProofStatement* traveller = proof_data[index]->getParent();
  for(; traveller != NULL; depth++, traveller = traveller->getParent());
  
  if(proof_data[index]->isAssumption() && index > last_premise)
  {
    //This is a subproof assumption, print an empty line before it
    cout << "   ";
    for(int i = 0; i <= depth-1; i++)
      cout << ']';
    cout << "\n";
  }
  
  //Print the line number and indent
  cout << (index+1) << ((index < 99)?((index < 9)?"  ":" "):""); //TODO: more robust formatting for this
  for(int i = 0; i <= depth; i++)
    cout << ']';
    
  //Print the line
  char* display_string = proof_data[index]->createDisplayString();
  cout << " " << display_string << "\n";
  delete [] display_string;
  
  if(proof_data[index]->isAssumption() && index > last_premise)
  {
    //This is a subproof assumption, print a separator after it
    cout << "   ";
    for(int i = 0; i <= depth; i++)
      cout << ']';
    cout << "---\n";
  }
}

char* Proof::createGoalString()
{
  if(goal == NULL)
  {
    char* empty_goal = new char[1];
    *empty_goal = '\0';
    return empty_goal;
  }
  
  return goal->createDisplayString();
}

void Proof::createPremiseStrings(std::vector<char*>& premise_strings)
{
  for(int i = 0; i <= last_premise; i++)
    premise_strings.push_back(proof_data[i]->getStatementData()->createDisplayString());
}
