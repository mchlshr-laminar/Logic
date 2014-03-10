#include "Proof/ProofReader.hpp"
#include <cstdlib>
#include <utility>

using std::cout;
using std::cerr;
using std::endl;
using std::stringstream;
using std::string;
using std::ifstream;
using std::map;
using std::pair;
using std::vector;

//Sets the proof object to store data in.
void ProofReader::setTarget(Proof* new_target)
{
  target = new_target;
  new_line_needed = true;
  derivation_started = false;
}

//Reads the file with the given name to the proof object.
bool ProofReader::readFile(const char* filename)
{
  line_number_offset = 0;
  line_number_translation.clear();
  
  if(target == NULL)
  {
    cerr << "Error: no proof to read file " << filename << "into \n";
    return false;
  }
  reader.open(filename);
  if(!reader.is_open())
  {
    cerr << "Error: file " << filename << " could not be opened\n";
    return false;
  }
  
  while(!reader.eof())
  {
    char* temp = readLine();
    char* line = temp; //this pointer moves
    while(*line == ' ' || *line == '\t') line++;
    
    if(line == NULL)
    {
      cerr << "Error: problem reading file " << filename << "\n";
      return false;
    }
    if(strcmp(line, "") == 0) break;
    
    if(strncmp(line, PREMISE_COMMAND, 4) == 0)
    {
      if(derivation_started)
      {
        cerr << "Error: premise after start of derivation in file " << filename << "\n";
        return false;
      }
      pre(line+3);
    }
    else if(strncmp(line, PROOFLINE_COMMAND, 4) == 0)
    {
      derivation_started = true;
      if(!lin(line+3))
      {
         malformedLine(filename, line);
         return false;
      }
    }
    else if(strncmp(line, SUBPROOF_COMMAND, 4) == 0)
    {
      derivation_started = true;
      sub(line+3);
    }
    else if(strcmp(line, SUBPROOF_END_COMMAND) == 0)
    {
      derivation_started = true;
      end(line+3);
    }
    else if(strncmp(line, GOAL_DEF_COMMAND, 4) == 0)
    {
      gol(line+3);
    }
	else if(strncmp(line, EQUIVALENCE_LEMMA_COMMAND, 4) == 0)
	{
    if(!equ(line+3))
    {
       malformedLine(filename, line);
       return false;
    }
	}
	else if(strncmp(line, INFERENCE_LEMMA_COMMAND, 4) == 0)
	{
    if(!inf(line+3))
    {
       malformedLine(filename, line);
       return false;
    }
	}
    else
    {
      cerr << "Error: unrecognized command in line: " << line << " from file " << filename << "\n";
      return false;
    }
    delete [] temp;
  }
  
  reader.close();
  return true;
}

//Reads, stores, and returns one line from the input file. Removes a
//terminating '\r' if present to account for DOS filetypes.
char* ProofReader::readLine()
{
  linestream.str("");
  char* temp = new char[50];
  do
  {
    reader.clear();
    reader.getline(temp, 50);
    linestream << temp;
  } while(reader.fail() && !reader.eof() && !reader.bad());
  delete [] temp;
  
  if(reader.fail()) return NULL;
  string tempstr = linestream.str();
  temp = new char[tempstr.size()+1];
  strcpy(temp, tempstr.c_str());
  if(tempstr.size() > 0 && temp[tempstr.size()-1] == '\r')
    temp[tempstr.size()-1]='\0'; //DOS File types
  return temp;
}

//Input line was a premise command.
bool ProofReader::pre(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  line_number_translation[line_number_translation.size()+1] =
    line_number_translation.size()-line_number_offset;
  
  target->addPremiseLine();
  target->setStatement(input);
  return true;
}

//input command was a line command
bool ProofReader::lin(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  if(new_line_needed)
  {
    target->addLine();
    extendLineNumberTranslation();
  }
  
  char* temp = strtok(input, ":");
  if(temp == NULL) return false;
  target->setStatement(temp);
  
  temp = strtok(NULL, ":");
  if(temp == NULL) return false;
  target->setJustification(temp);
  
  temp = strtok(NULL, " ");
  while(temp != NULL)
  {
    int ant = atoi(temp);
    if(ant > 0 && ant <= (int)line_number_translation.size())
      ant = line_number_translation[ant];
    target->toggleAntecedent(ant);
    temp = strtok(NULL, " ");
  }
  new_line_needed = true;
  return true;
}

//Input command was a subproof command
bool ProofReader::sub(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  extendLineNumberTranslation();
  
  target->addSubproofLine();
  target->setStatement(input);
  new_line_needed = true;
  return true;
}

//Et cetera
bool ProofReader::end(char* input)
{
  target->endSubproof();
  new_line_needed = false;
  line_number_offset++;
  extendLineNumberTranslation();
  return true;
}

bool ProofReader::gol(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  line_number_offset++;
  extendLineNumberTranslation();
  target->setGoal(input);
  return true;
}

//TODO: add some sorta loop prevention in lemma commands
//pass depth limit to ProofReader & decrement?

//Make an equivalence lemma
bool ProofReader::equ(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  line_number_offset++;
  extendLineNumberTranslation();
  
  char* equivalence_name = strtok(input, ":");
  if(equivalence_name == NULL || strcmp(equivalence_name, "") == 0) return false;
  
  //getting filenames
  char* direction_1_filename = strtok(NULL, ":");
  char* direction_2_filename = strtok(NULL, "\r\n");
  if(direction_1_filename == NULL || strcmp(direction_1_filename, "") == 0) return false;
  if(direction_2_filename == NULL || strcmp(direction_2_filename, "") == 0) return false;
  
  //reading files
  Proof direction_1, direction_2;
  ProofReader direction_1_reader, direction_2_reader;
  direction_1_reader.setTarget(&direction_1);
  direction_2_reader.setTarget(&direction_2);
  if(!direction_1_reader.readFile(direction_1_filename) || !direction_2_reader.readFile(direction_2_filename))
    return false;
  
  //Print and verify proofs
  cout << "Proofs for lemma \"" << equivalence_name << "\" follow:\nFirst Proof:\n";
  direction_1.printProof();
  bool dir_1_verified = direction_1.verifyProof();
  cout << "Second Proof:\n";
  direction_2.printProof();
  bool dir_2_verified = direction_2.verifyProof();
  if(!dir_1_verified || !dir_2_verified)
  {
    cout << "Equivalence lemma \"" << equivalence_name << "\" was not successfully proven.\n";
    cout << "The " << (dir_1_verified?"second":"first") << " proof was incorrect.\n";
    cout << "Lines that rely on this equivalence will appear as unjustified.\n-------------------------\n";
    return true;
  }
  
  //Get the premises
  char* form_1 = direction_1.createGoalString();
  char* form_2 = direction_2.createGoalString();
  vector<char*> premise_1, premise_2;
  direction_1.createPremiseStrings(premise_1);
  direction_2.createPremiseStrings(premise_2);
  
  //Check that the proofs are of the proper form & match each other.
  bool return_value = true;
  if(matchEquivalenceForms(form_1, form_2, premise_1, premise_2))
  {
    if(!target->addEquivalenceRule(form_1, form_2, equivalence_name))
    {
      cerr << "Error: justification rule named \"" << equivalence_name << "\" already exists\n";
      return_value = false;
    }
    cout << "-------------------------\n";
  }
  else cout << "Lines that rely on this equivalence will appear as unjustified.\n-------------------------\n";
  
  delete [] form_1;
  delete [] form_2;
  for(unsigned int i = 0; i < premise_1.size(); i++)
    delete [] premise_1[i];
  for(unsigned int i = 0; i < premise_2.size(); i++)
    delete [] premise_2[i];
  
  return return_value;
}

//Create an inference lemma
bool ProofReader::inf(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  line_number_offset++;
  extendLineNumberTranslation();
  
  //Names of things
  char* inference_name = strtok(input, ":");
  if(inference_name == NULL || strcmp(inference_name, "") == 0) return false;
  
  char* lemma_file_name = strtok(NULL, "\r\n");
  if(lemma_file_name == NULL || strcmp(lemma_file_name, "") == 0) return false;
  
  //Read file
  Proof lemma_proof;
  ProofReader lemma_proof_reader;
  lemma_proof_reader.setTarget(&lemma_proof);
  if(!lemma_proof_reader.readFile(lemma_file_name)) return false;
  
  //Print & verify proof
  cout << "Proof for lemma \"" << inference_name << "\" follows:\n";
  lemma_proof.printProof();
  if(!lemma_proof.verifyProof())
  {
    cout << "Inference lemma \"" << inference_name << "\" was not successfully proven.\n";
    cout << "Lines that rely on this inference will appear as unjustified.\n-------------------------\n";
    return true;
  }
  
  //Check existence of goal
  char* lemma_goal = lemma_proof.createGoalString();
  if(strcmp(lemma_goal, "") == 0)
  {
    delete [] lemma_goal;
    cout << "Inference lemma \"" << inference_name << "\" has no consequent and consequently could not be created.\n";
    cout << "Lines that rely on this inference will appear as unjustified.\n-------------------------\n";
    return true;
  }
  vector<char*> lemma_premises;
  lemma_proof.createPremiseStrings(lemma_premises);
  
  //Create rule
  bool return_value = true;
  if(!target->addInferenceRule(lemma_premises, lemma_goal, inference_name))
  {
    cerr << "Error: justification rule named \"" << inference_name << "\" already exists\n";
    return_value = false;
  }
  cout << "-------------------------\n";
  delete [] lemma_goal;
  for(unsigned int i = 0; i < lemma_premises.size(); i++)
    delete [] lemma_premises[i];
  
  return return_value;
}

void ProofReader::malformedLine(const char* filename, char* input)
{
	cerr << "Error in " << filename << ": line " << input << " is malformed\n";
}

//For translation between file line number & proof line number.
void ProofReader::extendLineNumberTranslation()
{
  line_number_translation[line_number_translation.size()+1] =
    line_number_translation.size()-line_number_offset;
}

bool ProofReader::matchEquivalenceForms(char* form_1, char* form_2, vector<char*> premise_1, vector<char*> premise_2)
{
  if(strcmp(form_1, "") == 0 || strcmp(form_2, "") == 0)
    cout << "One of the proofs had no goal set.\n";
  else if(premise_1.size() != 1)
    cout << "The first proof had " << ((premise_1.size()>1)?"more":"fewer") << " than 1 premise.\n";
  else if(premise_2.size() != 1)
    cout << "The second proof had " << ((premise_2.size()>1)?"more":"fewer") << " than 1 premise.\n";
  else if(strcmp(premise_1[0], form_2) != 0 || strcmp(premise_2[0], form_1) != 0)
    cout << "The premise of one proof does not match the goal of the other.\n";
  else return true;
  
  return false;
}

