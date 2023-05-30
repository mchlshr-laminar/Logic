#ifndef __EQUIV_RULES_H_
#define __EQUIV_RULES_H_

class EquivalenceRule;

#include "Justification.hpp"
#include "StatementTree.hpp"
#include "ProofStatement.hpp"
#include <utility>
#include <list>

/// <summary>
/// Represents a pair of syntax trees which are logically equivalent.
/// </summary>
typedef std::pair<StatementTree*, StatementTree*> equiv_pair;

//Checks justification using an equivalence rule, i.e. can be applied
//to subsentences and can be used in either direction.

/// <summary>
/// For justification rules based on pairs of logically equivalent sentences.
/// For example, the sentences "a" and "a&a" are logically equivalent (this
/// is idempotence). These equivalencies can be applied in either direction,
/// and one rule can have multiple pairs of equivalent forms that can be used
/// ("a" and "a|a" is also idempotence). These rules can also be applied to
/// subtrees of a proof line, for instance the line "a&(b|b)" could be 
/// justified from the line "a&b" based on idempotence.
/// </summary>
class EquivalenceRule : public Justification
{
  private:
  std::list<equiv_pair> equivalent_pairs;
  
  /// <summary>
  /// Checks whether two logical sentences are equivalent by way of application
  /// of this rule. This means that wherever the trees are not the same, the
  /// difference between them corresponds to one of the pairs of equivalent
  /// forms in this rule (the structure in one tree matches form 1 while the
  /// structure in the other matches the equivalent form 2, with the subtrees
  /// that correspond to the same sentence variables in the forms being 
  /// equivalent). Multiple applications of the equivalence rule are allowed.
  /// </summary>
  /// <param name="tree1">The first sentence</param>
  /// <param name="tree2">The second sentence</param>
  /// <returns>
  ///   True if the first and second sentences are logically equivalent by
  ///   use of this rule.
  /// </returns>
  bool areEquivalent(StatementTree* tree1, StatementTree* tree2);

  /// <summary>
  /// Determines whether a statement tree can be considered to be an instance
  /// of one form of an equivalent pair while respecting any existing bindings
  /// between sentence variables in the form and statement trees.
  /// 
  /// If the form to match with is a sentence variable:
  ///   If that variable is already bound, a match can be made if the target 
  ///   sentence is equivalent to the bound sentence (potentially using this 
  ///   equivalence rule again).
  ///   Otherwise match by binding the target sentence to that variable.
  /// 
  /// If the form is not a sentence variable, a match can be made if:
  ///   The node type and negation flag at the root of the target and the form
  ///   match, AND the left and right children of the target can match the
  ///   left and right children of the form.
  /// </summary>
  /// <param name="target">Sentence to try to match with the form</param>
  /// <param name="form">Form from an equivalent pair to match against</param>
  /// <param name="binds">
  ///   Contains existing bindings between sentence variables and statement
  ///   trees which must be respected when looking for a match. If a match is
  ///   made, this will be updated to include any new bindings needed to make
  ///   that match.
  /// </param>
  /// <returns>
  ///   True if a match between the target and the form can be made
  /// </returns>
  bool match(StatementTree* target, StatementTree* form, bind_map& binds);

  /// <summary>
  /// Toggles the negation flag on the root node of both forms of an 
  /// equivalent pair so that the root node negation of form 1 of the pair
  /// matches the root node negation of a target statement tree which we are
  /// trying to match with the pair.
  /// </summary>
  /// <param name="target">Statement tree to match the root negation of</param>
  /// <param name="source">
  ///   Equivalent pair to potentially toggle the negation of.
  /// </param>
  void matchFormOneNegation(StatementTree* target, equiv_pair& source);
  
  public:
  EquivalenceRule(const char* name) : Justification(name)
  {}
  virtual ~EquivalenceRule();
  
  /// <summary>
  /// Adds another pair of equivalent sentences which can be used when 
  /// applying this rule.
  /// </summary>
  /// <param name="form1">First equivalent form</param>
  /// <param name="form2">Second equivalent form</param>
  void addEquivalentPair(const char* form1, const char* form2);
  
  /// <summary>
  /// Application of an equivalence rule is considered justified if there is
  /// exactly one antecedent, and that antecedent is equivalent to the
  /// consequent (by way of areEquivalent).
  /// </summary>
  /// <param name="consequent">
  ///   The proposed consequent of applying the equivalence.
  /// </param>
  /// <param name="antecedents">
  ///   The antecedent on which the rule will be applied. If more than one is
  ///   listed, then the justification fails.
  /// </param>
  /// <returns>
  ///   True if the proposed consequent is actually achieved by that 
  ///   application.
  /// </returns>
  bool isJustified(StatementTree& consequent, antecedent_list& antecedents);
};

#endif
