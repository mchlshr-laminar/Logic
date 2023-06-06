#ifndef __PROOF_H_
#define __PROOF_H_

#include "ProofStatement.hpp"
#include "SubProof.hpp"
#include "StatementTree.hpp"
#include "Justification.hpp"
#include "InferenceRules.hpp"
#include "EquivalenceRules.hpp"
#include "AggregateJustification.hpp"
#include <cstring>
#include <map>
#include <vector>
#include <string>

/// <summary>
/// For the list of lines in the proof.
/// </summary>
typedef std::vector<ProofStatement*> proof_list;

/// <summary>
/// Stores a proof, consisting of a list of ProofStatements for the lines in the proof. SubProof
/// instances are not directly included in this list, just the lines within them.
/// 
/// One line of the proof will have focus, and any operations are relative to that line, e.g.
/// adding a new line will insert it after the focused line. This is in anticipation of potentially
/// making an interactive UI.
/// </summary>
class Proof
{
  private:
  int current_position;
  int last_premise;
  proof_list proof_data;
  Assumption premise_just;
  StatementTree* goal;
  
  public:
  Proof();
  ~Proof();
  
  /// <summary>
  /// Set which line of the proof has focus. Position is zero-indexed for all lines of the proof,
  /// including premises, subproof assumptions, and lines within subproofs (but not the goal
  /// statement or the subproof objects themselves). Position can also be set to -1 to insert
  /// lines before the first existing line.
  /// </summary>
  /// <param name="new_position">
  ///   Index of the line to focus, in range -1 to proof_data.size()-1
  /// </param>
  void setPosition(int new_position);

  /// <summary>
  /// Set the sentence on the currently focused line. If focus is before the beginning of the
  /// proof, does nothing.
  /// </summary>
  /// <param name="statement_string">
  ///   Sentence to put on this line. Will be parsed into a syntax tree.
  /// </param>
  void setStatement(const char* statement_string);

  /// <summary>
  /// Sets the goal of the proof. The proof is successful if every line is justified and there is
  /// a derived line (which is not within a subproof) which matches the goal.
  /// </summary>
  /// <param name="goal_string">
  ///   Sentence for the goal of the proof. Will be parsed into a syntax tree.
  /// </param>
  void setGoal(const char* goal_string);
  
  /// <summary>
  /// Adds a new line to the proof after the currently focused line. If focus is on -1, this means
  /// adding at the start of the proof. However if there are any premise lines in the proof focus
  /// will first be shifted to the last of them, so the new line will be after the premises.
  /// 
  /// If the focused line is within a subproof, the new line will also be in that subproof. The
  /// new line will initially have no sentence, justification, or antecedents. Focus will be
  /// moved to the new line.
  /// </summary>
  void addLine();

  /// <summary>
  /// Adds a new premise line to the proof. If the current focus is on a premise, the new line will
  /// be after it. If focus is after the premises the new premise will be at the end of the
  /// premises. The new line will not have a sentence, but will have the premise justification.
  /// Focus will move to the new premise line.
  /// </summary>
  void addPremiseLine();

  /// <summary>
  /// Starts a new subproof and creates an assumption line for it. This line will use an Assumption
  /// justification. The subproof will be after the currently focused line, or after the last
  /// premise if focus is in the premises. If the focused line is already in a subproof, the new
  /// one will be a sub-subproof of that one. Focus will move to the new assumption line.
  /// </summary>
  void addSubproofLine();

  /// <summary>
  /// Adds a new proof line as per addLine, but if the current focus is within a subproof the new
  /// one will be after the end of that subproof. If there's an encompassing subproof the new line
  /// will be in that one.
  /// </summary>
  void endSubproof();

  /// <summary>
  /// Removes the currently focused line. If that line is the assumption of a subproof, will
  /// remove all the contents of that subproof as well.
  /// </summary>
  void removeLine();

  /// <summary>
  /// Sets the justification on the currently focused line, based on the name of the rule.
  /// If the focused line is a premise or assumption, or there's no justification rule by that
  /// name, does nothing.
  /// </summary>
  /// <param name="justification_name">
  ///   Name of the justification rule to search for.
  /// </param>
  void setJustification(const char* justification_name);

  /// <summary>
  /// Toggles whether a given line is an antecedent of the currently focused line. If the focused
  /// line is a premise or assumption, or focus is before the first line, does nothing. Also does
  /// nothing if the given line is not before the current focus.
  /// </summary>
  /// <param name="antecedent_index">
  ///   Line index of the antecedent to toggle, in range 0 to current focus - 1.
  /// </param>
  void toggleAntecedent(int antecedent_index);

  /// <summary>
  /// Adds an equivalence rule to the list of justifications usable in this proof.
  /// TODO: Maybe shouldn't be in Proof.
  /// </summary>
  /// <param name="form1">First equivalent form</param>
  /// <param name="form2">Second equivalent form</param>
  /// <param name="name">Name of the new justification rule</param>
  /// <returns>True if the rule was added</returns>
  bool addEquivalenceRule(const char* form1, const char* form2, const char* name);

  /// <summary>
  /// Adds an inference rule to the list of justifications usable in this proof..
  /// TODO: Maybe shouldn't be in Proof.
  /// </summary>
  /// <param name="antecedents">List of required antecedent forms</param>
  /// <param name="goal">Consequent form</param>
  /// <param name="name">Name of the new justification rule</param>
  /// <returns>True if the rule was added</returns>
  bool addInferenceRule(const std::vector<char*>& antecedents, const char* goal, const char* name);
  
  //TODO: A proof that ends with a subproof will cause verification to fail due to the empty
  //statement after it.I'm not sure why anyone would end a proof with a subproof, but this is not ideal.

  /// <summary>
  /// Checks whether the proof is successful, which means all lines are well-formed and justified.
  /// If a goal is set, also means there is a derived line containing the goal (not in a subproof).
  /// If verification is not successful, prints the reason(s) it failed to the console.
  /// </summary>
  /// <returns>True if verification succeeded</returns>
  bool verifyProof();

  /// <summary>
  /// Prints the proof to the console.
  /// </summary>
  void printProof();

  /// <summary>
  /// Create a string representing the goal of the proof. If no goal is set, returns an empty string.
  /// Otherwise returns the sentence for the goal statement. Used in adding lemmas.
  /// </summary>
  /// <returns>Goal sentence</returns>
  char* createGoalString();

  /// <summary>
  /// Creates a list of strings representing the premises of a proof. Used in adding lemmas
  /// </summary>
  /// <param name="premise_strings">
  ///   For each premise line of the proof, contains the sentence of its statement.
  /// </param>
  void createPremiseStrings(std::vector<char*>& premise_strings);
  
  private:

  /// <summary>
  /// Helper for printProof, prints one line of the proof. Indents the line based on how many
  /// levels of subproof it's in, then prints that line's display string.
  /// </summary>
  /// <param name="index">Line index to print</param>
  void printProofLine(int index);

  /// <summary>
  /// Gets an iterator of proof_data for inserting a new line. This means an iterator to the line
  /// after the current focus, or end() if focus is on the last line.
  /// </summary>
  /// <returns>Iterator for new line insertion</returns>
  proof_list::iterator getLineInsertionIterator();
};

#endif

