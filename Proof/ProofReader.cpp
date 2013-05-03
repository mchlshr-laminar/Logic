#include "ProofReader.hpp"
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

void ProofReader::setTarget(Proof* new_target)
{
  target = new_target;
  new_line_needed = true;
  derivation_started = false;
}

bool ProofReader::readFile(const char* filename)
{
  line_number_offset = 0;
  line_number_translation.clear();
  
  if(target == NULL)
  {
    cerr << "Error: no proof to read to\n";
    return false;
  }
  reader.open(filename);
  if(!reader.is_open())
  {
    cerr << "Error: file could not be opened\n";
    return false;
  }
  
  while(!reader.eof())
  {
    char* temp = readLine();
    char* line = temp;
    while(*line == ' ' || *line == '\t') line++;
    
    if(line == NULL)
    {
      cerr << "Error: problem reading file\n";
      return false;
    }
    if(strcmp(line, "") == 0) break;
    
    if(strncmp(line, PREMISE_COMMAND, 4) == 0)
    {
      if(derivation_started)
      {
        cerr << "Error: premise after start of derivation\n";
        return false;
      }
      pre(line+3);
    }
    else if(strncmp(line, PROOFLINE_COMMAND, 4) == 0)
    {
      derivation_started = true;
      lin(line+3);
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
    else
    {
      cerr << "Error: unrecognized command in line: " << line << "\n";
      return false;
    }
    delete [] temp;
  }
  
  reader.close();
  return true;
}

char* ProofReader::readLine()
{
  linestream.str("");
  char* temp = new char[50];
  do
  {
    reader.getline(temp, 50);
    linestream << temp;
  } while(linestream.fail() && !linestream.eof() && !linestream.bad());
  delete [] temp;
  
  if(linestream.fail() || linestream.bad()) return NULL;
  string tempstr = linestream.str();
  temp = new char[tempstr.size()+1];
  strcpy(temp, tempstr.c_str());
  return temp;
}

bool ProofReader::pre(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  line_number_translation[line_number_translation.size()+1] =
    line_number_translation.size()-line_number_offset;
  
  target->addPremiseLine();
  target->setStatement(input);
  return true;
}

bool ProofReader::lin(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  if(new_line_needed)
  {
    target->addLine();
    line_number_translation[line_number_translation.size()+1] =
      line_number_translation.size()-line_number_offset;
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

bool ProofReader::sub(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  line_number_translation[line_number_translation.size()+1] =
    line_number_translation.size()-line_number_offset;
  
  target->addSubproofLine();
  target->setStatement(input);
  new_line_needed = true;
  return true;
}

bool ProofReader::end(char* input)
{
  target->endSubproof();
  new_line_needed = false;
  line_number_offset++;
  line_number_translation[line_number_translation.size()+1] =
    line_number_translation.size()-line_number_offset;
  return true;
}

bool ProofReader::gol(char* input)
{
  while(*input == ' ' || *input == '\t') input++;
  line_number_offset++;
  line_number_translation[line_number_translation.size()+1] =
    line_number_translation.size()-line_number_offset;
  target->setGoal(input);
  return true;
}

