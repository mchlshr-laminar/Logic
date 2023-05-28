#ifndef __SUBPROOF_H_
#define __SUBPROOF_H_

class SubProof;

#include "ProofStatement.hpp"
#include "Justification.hpp"

/// <summary>
/// Subclass of ProofStatement for a subproof. A subproof contains one
/// statement which is assumed, then a sequence of statements which may use
/// that assumption as an antecedent. Statements after the end of the subproof
/// can't use any statement in it as an antecedent, but they can use the
/// subproof itself as one, in which case the justification rule specifies the
/// form the assumption must take, as well as the form of a derived line which
/// must appear (directly) in the subproof.
/// </summary>
class SubProof : public ProofStatement
{
  private:
  static Assumption subproof_assumption;
  statement_set contents;
  ProofStatement* assumption;
  
  public:
  /// <summary>
  /// Construct a subproof. The passed string will be parsed into a syntax
  /// tree for the subproof's assumption statement.
  /// </summary>
  /// <param name="input">Logical sentence assumed by the subproof</param>
  SubProof(const char* input);

  /// <summary>
  /// Construct a subproof. The input syntax tree will be copied into the
  /// assumption statement
  /// </summary>
  /// <param name="input">Subproof assumption</param>
  SubProof(StatementTree* input);
  virtual ~SubProof();
  
  /// <summary>
  /// Gets the syntax tree for the statement on a proof line.
  /// </summary>
  /// <returns>NULL for a subproof</returns>
  StatementTree* getStatementData();

  /// <summary>
  /// Gets the assumption this subproof is based on.
  /// </summary>
  /// <returns>Syntax tree of the assumption.</returns>
  StatementTree* getAssumption();

  /// <summary>
  /// Gets the proof line for this subproof's assumption, which should
  /// generally be the first line withing the subproof. This line should
  /// use the static subproof_assumption justification.
  /// </summary>
  /// <returns>Proof line of the assumption</returns>
  ProofStatement* getAssumptionStatement();

  /// <summary>
  /// Check whether this is a valid, justified line in the proof. Sets the
  /// failure type flat to NO_FAILURE if it is, otherwise sets it to what issue
  /// prevents it from being justified. For a subproof, this just checks
  /// whether the assumption is well formed. The failure type flag will
  /// therefore be set either to NO_FAILURE or INVALID_STATEMENT.
  /// </summary>
  /// <returns>
  ///   True if the subproof's assumption is well formed, false othewise.
  /// </returns>
  bool isJustified();
  
  /// <summary>
  /// Gets the derived lines in this subproof, for checking whether it's a
  /// usable antecedent for a justification rule.
  /// </summary>
  /// <returns>
  ///   Unordered set of the proof lines immediately in this subproof, not
  ///   including the assumption statement.
  /// </returns>
  statement_set* getSubproofContents();

  /// <summary>
  /// Gets the (internal) line index of a proof line. For a subproof this is
  /// equivalent to the index of the subproof's assumption line.
  /// </summary>
  /// <returns>Line index of the subproof's assumption</returns>
  int getLineIndex();

  /// <summary>
  /// Generates a string to represent a proof line as text. For a subproof,
  /// this just shows the assumption line.
  /// </summary>
  /// <returns>
  ///   Display string for the proof line of this subproof's assumption.
  /// </returns>
  char* createDisplayString();
  
  /// <summary>
  /// Change the sentence in a proof line to a new one. For a subproof this
  /// changes the sentence in the assumption line.
  /// </summary>
  /// <param name="input">
  ///   Logical sentence to use as the subproof assumption. Will be parsed
  ///   into a syntax tree.
  /// </param>
  void rewrite(const char* input);

  /// <summary>
  /// Change the sentence in a proof line to a new one. For a subproof this
  /// changes the sentence in the assumption line.
  /// </summary>
  /// <param name="input">
  ///   Syntax tree for the subproof assumption statement. Will be copied.
  /// </param>
  void rewrite(StatementTree* input);
  
  /// <summary>
  /// Toggles whether another proof line is an antecedent of this one. For a
  /// subproof this does nothing, as subproofs have no antecedents.
  /// </summary>
  /// <param name="ant">Other proof statement</param>
  /// <returns>false</returns>
  bool toggleAntecedent(ProofStatement* ant);

  /// <summary>
  /// Toggples whether or not a derived proof line is within this subproof.
  /// This is called by the setParent function of the new or departing child,
  /// so it does not update the parent pointer in the child. Included as a
  /// virtual function in ProofStatement for convenience, but does nothing
  /// for non-subproof statements. Subproof assumption lines use rewrite
  /// to change their contents rather than adding/removing.
  /// </summary>
  /// <param name="childStatement">
  ///   Proof line to add or remove from this subproof.
  /// </param>
  /// <returns>
  ///   True if the specified proof line was an existing child of the subproof,
  ///   i.e. it has been removed. False if it's a new child that has been
  ///   added.
  /// </returns>
  bool toggleChild(ProofStatement* childStatement);
};

#endif

