#ifndef __PROOF_READER_H_
#define __PROOF_READER_H_

#include "Proof.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>

#define PREMISE_COMMAND "pre "
#define PROOFLINE_COMMAND "lin "
#define SUBPROOF_COMMAND "sub "
#define SUBPROOF_END_COMMAND "end"
#define GOAL_DEF_COMMAND "gol "

//Reads a proof from an input file.
class ProofReader
{
  private:
  std::ifstream reader;
  std::stringstream linestream;
  Proof* target;
  bool new_line_needed;
  bool derivation_started;
  std::map<int, int> line_number_translation;
  int line_number_offset;
  
  public:
  ProofReader() : target(NULL)
  {}
  
  void setTarget(Proof* new_target);
  bool readFile(const char* filename);
  
  private:
  char* readLine();
  
  bool pre(char* input);
  bool lin(char* input);
  bool sub(char* input);
  bool end(char* input);
  bool gol(char* input);
};

#endif

