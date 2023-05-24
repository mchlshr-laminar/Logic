#ifndef __PROOF_STATEMENT_H_
#define __PROOF_STATEMENT_H_

#include <list>
#include <set>

/// <summary>
/// Class for a line in the proof or a subproof, either an assumption, the goal,
/// or a line of derivation. Subproofs use a subclass of this (SubProof).
/// A proof line contains the statement, the rule by which that statement is
/// justified, and the antecedent proof lines on which that justification is
/// based.
/// </summary>
class ProofStatement;

/// <summary>
/// Type for a list of other proof lines, for use as the antecedents on which
/// this line's justification is based.
/// </summary>
typedef std::list<ProofStatement*> antecedent_list;
typedef std::set<ProofStatement*> statement_set;

#include "StatementTree.hpp"
#include "Justification.hpp"

//Represents one statement in the proof
class ProofStatement
{
  public:

  /// <summary>
  /// The possible reasons a line in the proof may not be considered valid:
  /// -The statement is not well formed
  /// -No justification rule has been set
  /// -The supplied antecedents do not support the specified justification
  /// </summary>
  enum failure_type_t { NO_FAILURE, INVALID_STATEMENT, NO_JUSTIFICATION,
    JUSTIFICATION_FAILURE};
  protected:

  ProofStatement* parent;
  int line_index;
  
  Justification* reason;
  StatementTree* data;
  antecedent_list antecedents;
  bool is_assumption;
  failure_type_t fail_type;

public:

    /// <summary>
    /// Construct a proof line for a logical sentence. The sentence will be
    /// parsed into a syntax tree.
    /// </summary>
    /// <param name="input">Sentence to parse</param>
    /// <param name="is_assump">True if this is a premise or subproof assumption</param>
    ProofStatement(const char* input, bool is_assump = false);

    /// <summary>
    /// Construct a proof line for a syntax tree. The existing syntax tree will
    /// be copied.
    /// </summary>
    /// <param name="input">Syntax tree to copy.</param>
    /// <param name="is_assump">True if this is a premise or subproof assumption</param>
    ProofStatement(StatementTree* input, bool is_assump = false);
    virtual ~ProofStatement();

    /// <summary>
    /// Generates a string to print this line as text. Note that this doesn't
    /// include printing the line number or indenting for subproofs; those are
    /// handled in Proof.printProof.
    /// </summary>
    /// <returns>
    ///   Separated by spaces:
    ///       -Statement sentence, per StatementTree.createDisplayString
    ///       -Name of the justification rule, or "Not Justified"
    ///       -Comma-delimited list of antecedent line numbers
    ///  </returns>
    virtual char* createDisplayString();

#pragma region Justification
public:
    /// <summary>
    /// Check whether this is a valid, justified line in the proof. Sets the
    /// failure type flat to NO_FAILURE if it is, otherwise sets it to what issue
    /// prevents it from being justified.
    /// </summary>
    /// <returns>
    ///   True if the line's statement is well-formed, has a justification rule,
    ///   and the specified antecedents support this statement by that rule.
    ///   False otherwise. 
    /// </returns>
    virtual bool isJustified();

    /// <summary>
    /// Is this is a premise line or the assumption of a subproof?
    /// </summary>
    /// <returns>True if it is, false otherwise</returns>
    bool isAssumption();

    /// <summary>
    /// If a line is not justified, this says what the problem with it is.
    /// Assumes isJustified has already been checked, as that's where the flag
    /// is set.
    /// </summary>
    /// <returns>
    ///   NO_FAILURE if the line is justified or isJustified hasn't been checked.
    ///   See failure_type_t for reasons it could fail.
    /// </returns>
    failure_type_t getFailureType();

    /// <summary>
    /// The inference or logical equivalence rule used to justify this line of
    /// the proof. For a premise line or subproof assumption, this will be an
    /// instance of the Assumption class.
    /// </summary>
    /// <returns>Rule by which this line is justified</returns>
    Justification* getJustification();

    /// <summary>
    /// Sets the rule of inference used to justify this line. If this is an
    /// assumption line, attempting to set the justification to null will not
    /// do anything.
    /// </summary>
    /// <param name="new_reason">Justification for this line</param>
    void setJustification(Justification* new_reason);

protected:

#pragma endregion

#pragma region Subproofs
public:
    /// <summary>
    /// Used by the SubProof subclass, returns the syntax tree for the assumption
    /// that subproof is based on.
    /// </summary>
    /// <returns>Null for normal statements, syntax tree for subproofs.</returns>
    virtual StatementTree* getAssumption();

    /// <summary>
    /// For subproof lines, this points to the innermost subproof this line is
    /// part of.
    /// </summary>
    /// <returns>SubProof this line is in. Null if this isn't in a subproof.</returns>
    ProofStatement* getParent();

    /// <summary>
    /// Puts this proof line in a subproof. If it was already in a subproof, it
    /// will be removed from direct membership of the old one. Updates child
    /// lists in both relevant subproofs.
    /// </summary>
    /// <param name="new_parent">
    ///   Subproof to add this to. Should be of type SubProof.
    /// </param>
    void setParent(ProofStatement* new_parent);

    /// <summary>
    /// Used by SubProof. Returns the contents of that subproof.
    /// </summary>
    /// <returns>
    ///   Null for normal lines. Unordered set of lines in the subproof
    ///   for SubProof instances.
    /// </returns>
    virtual statement_set* getSubproofContents();

protected:
    /// <summary>
    /// Used by SubProof to add or remove a line from the subproof. Does nothing
    /// for normal proof lines.
    /// </summary>
    /// <param name="ch"></param>
    /// <returns></returns>
    virtual bool toggleChild(ProofStatement* childStatement);

#pragma endregion

#pragma region DataFunctions
public:
    /// <summary>
    /// Gets the syntax tree for the statement on this line of the proof.
    /// </summary>
    /// <returns>
    ///   Abstract syntax tree for this line. If this is a subproof,
    ///   returns null.
    /// </returns>
    virtual StatementTree* getStatementData();

    /// <summary>
    /// Changes the sentence at this line to a new one.
    /// </summary>
    /// <param name="input">Logical sentence. Will be parsed into a syntax tree.</param>
    virtual void rewrite(const char* input);

    /// <summary>
    /// Changes the sentence at this line to a new one.
    /// </summary>
    /// <param name="input">Syntax tree of the sentence. Will be copied.</param>
    virtual void rewrite(StatementTree* input);

    /// <summary>
    /// Toggles whether another proof line is in this line's list of antecedents.
    /// If the specified line is in subproofs which this line isn't, the
    /// antecedent that's actually toggled will be the innermost subproof parent
    /// of the specified line which is not in a separate subproof. This is because
    /// individual lines of a subproof are not valid antecedents for lines outside
    /// that subproof.
    /// </summary>
    /// <param name="ant">Antecedent to add or remove</param>
    /// <returns>
    ///   True if the antecedent was already in the list (i.e. it's been removed)
    ///   False otherwise.
    /// </returns>
    virtual bool toggleAntecedent(ProofStatement* ant);

    /// <summary>
    /// What line number in the proof is this? Note that is is the internal
    /// line number, which will be different from the line number as written
    /// in the input file. This number is zero-indexed and doesn't increment for
    /// the goal definition or subproof "end" statements.
    /// </summary>
    /// <returns>
    ///   Line number. For subproofs, returns the line number of
    ///   the assumption.
    /// </returns>
    virtual int getLineIndex();

    /// <summary>
    /// Sets this line's position in the proof. Note that this is the internal
    /// line index, and this function does not adjust any other line's index.
    /// </summary>
    /// <param name="i">Index of this line</param>
    void setLineIndex(int i);

protected:
    /// <summary>
    /// Returns the most immediate ancestor of another proof line which would
    /// be a permissible antecedent for this line, i.e. which is not the
    /// descendant of any subproofs this line is not also a descendant of. Starts
    /// by checking the specified line, and if it's not permissible, iterates up
    /// through its ancestors.
    /// </summary>
    /// <param name="antecedent">Proof line to find an ancestor of</param>
    /// <returns>
    ///   The parameter antecedent or one of its ancestors.
    /// </returns>
    ProofStatement* getRelevantAncestor(ProofStatement* antecedent);

    /// <summary>
    /// Helper function for getRelevantAncestor, which checks whether one given
    /// line would be acceptable. Operates by checking whether that line's
    /// immediate parent is also one of this line's ancestors, i.e. if that line
    /// is not in any additional subproofs.
    /// </summary>
    /// <param name="antecedent">Line to check</param>
    /// <returns>
    ///   True if the specified line could be used as an antecedent of this line.
    ///   False othewise.
    /// </returns>
    bool antecedentAllowable(ProofStatement* antecedent);

#pragma endregion
};

#endif
