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
#define EQUIVALENCE_LEMMA_COMMAND "equ "
#define INFERENCE_LEMMA_COMMAND "inf "

//Reads a proof from an input file.

/// <summary>
/// Reads a proof in an input file into a Proof object. The format of the
/// input file is specified in readme.txt.
/// </summary>
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
  
  /// <summary>
  /// Sets the Proof object to read the file into. This should be an empty
  /// proof.
  /// </summary>
  /// <param name="new_target">
  ///   Proof object to read the proof into.
  /// </param>
  void setTarget(Proof* new_target);

  /// <summary>
  /// Reads the input file into the Proof. Potential failure conditions are:
  /// -File doesn't exist or file IO error.
  /// -Proof object has not been set.
  /// -The proof or one of its lines is not formatted correctly.
  /// </summary>
  /// <param name="filename">File to read from</param>
  /// <returns>
  ///   True if the proof was successfully read. This does not mean the proof
  ///   is valid.
  /// </returns>
  bool readFile(const char* filename);
  
  private:

  /// <summary>
  /// Helper for readFile. Loads one line of the input file into a char*,
  /// to be parsed. The returned string will not contain the newline char,
  /// or the carriage return if it has a DOS line ending.
  /// </summary>
  /// <returns>Newly-allocated char*</returns>
  char* readLine();
  
#pragma region RegularProofLines

  /// <summary>
  /// Reads a premise line and inserts it into the proof.
  /// </summary>
  /// <param name="input">
  ///   Contents of the premise line, after the "pre" command. Should just
  ///   be the premise sentence with leading whitespace.
  /// </param>
  /// <returns>True</returns>
  bool pre(char* input);

  /// <summary>
  /// Reads a derived line and inserts it into the proof.
  /// </summary>
  /// <param name="input">
  ///   Contents of the proof line, after the "lin" command. Contains the
  ///   logical sentence, justification name, and list of antecedent lines.
  ///   Antecedents are listed by external line number, i.e. the actual line
  ///   in the input file (one-indexed).
  /// </param>
  /// <returns>
  ///   True if the statement and justification strings can be read.
  /// </returns>
  bool lin(char* input);

  /// <summary>
  /// Reads a line for a new subproof. Inserts the subproof and its assumption
  /// into the proof.
  /// </summary>
  /// <param name="input">
  ///   Subproof line, after the "sub" command. Contains the subproof's
  ///   assumption.
  /// </param>
  /// <returns>True</returns>
  bool sub(char* input);

  /// <summary>
  /// Reads a line to end a subproof. Ends the current innermost subproof in
  /// the Proof.
  /// </summary>
  /// <param name="input">
  ///   Input line after the "sub" command. Should be whitespace;
  ///   is ignored.
  /// </param>
  /// <returns>True</returns>
  bool end(char* input);

  /// <summary>
  /// Reads the proof's goal from the input file. If there's more than one
  /// goal line in the input file onlly the last one will matter.
  /// </summary>
  /// <param name="input">
  ///   Input line after the "gol" command. Contains the goal sentence.
  /// </param>
  /// <returns>True</returns>
  bool gol(char* input);
#pragma endregion

#pragma region Lemmas

  /// <summary>
  /// Reads an equivalence lemma declaration. Will read and verify the lemma
  /// proof files, and add the equivalence rule if that succeeds.
  /// </summary>
  /// <param name="input">
  ///   Input line after the "equ" command. Contains the rule name and the
  ///   two lemma proof file names.
  /// </param>
  /// <returns>
  ///   True if the line is well-formed and the lemma proof files were read
  ///   successfully, even if justification of the lemma fails.
  /// </returns>
  bool equ(char* input);

  /// <summary>
  /// Reads an inference rule lemma declaration, as per equ but for inference
  /// rules.
  /// </summary>
  /// <param name="input">
  ///   Input line after the "inf" command. Contains the rule name and the
  ///   lemma proof file name.
  /// </param>
  /// <returns>
  ///   True if the line is well-formed and the lemma proof file was read
  ///   successfully, even if justification of the lemma fails.
  /// </returns>
  bool inf(char* input);
#pragma endregion
  
  /// <summary>
  /// Outputs text to the error console describing a line that couldn't be
  /// read.
  /// </summary>
  /// <param name="filename">Name of the file the line is in</param>
  /// <param name="input">Contents of the line</param>
  void malformedLine(const char* filename, char* input);

  /// <summary>
  /// Adds another line to a mapping from external line numbers to internal
  /// line numbers. External line numbers start from 1 while internal ones
  /// start with 0, and any "gol" or "end" line in the input file does not
  /// count as an internal line.
  /// 
  /// While reading lines, any "gol" or "end" line should increment 
  /// line_number_offset to account for this. Each line read should call this.
  /// </summary>
  void extendLineNumberTranslation();

  /// <summary>
  /// Helper for the "equ" command. Checks that both proof files have a goal
  /// and exactly one premise, and the premise of one matches the goal of the
  /// other (and vice versa).
  /// </summary>
  /// <param name="form_1">Goal statement of proof 1</param>
  /// <param name="form_2">Goal statement of proof 2</param>
  /// <param name="premise_1">List of premises from proof 1</param>
  /// <param name="premise_2">List of premises from proof 2</param>
  /// <returns></returns>
  bool matchEquivalenceForms(char* form_1, char* form_2, std::vector<char*> premise_1, std::vector<char*> premise_2);
};

#endif

