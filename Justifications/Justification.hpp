#ifndef __JUSTIFICATION_H_
#define __JUSTIFICATION_H_

#include <map>
#include <list>
#include <cstring>

class Justification;

#include "StatementTree.hpp"
#include "ProofStatement.hpp"

/// <summary>
/// When checking whether a justification rule is appropriately applied, the
/// atomic propositions in the forms specified in the rule must correspond
/// with subtrees of the proof lines it's being applied to, and the same
/// proposition must correspond with an equivalent tree each time it appears.
/// This map stores that correspondance.
/// </summary>
typedef std::map<char, StatementTree*> bind_map;

/// <summary>
/// Used to check whether any of the listed antecedents are unused when
/// checking whether a rule can be applied.
/// </summary>
typedef std::map<ProofStatement*, int> statement_usage_map;

/// <summary>
/// Represents the reason why a logical statement can be considered a valid
/// part of the proof. The base class is abstract; the justification for any
/// particular line must either be a rule of deduction or the line must be
/// assumed as a premise either of the overall proof or of a subproof within
/// it. In the former case the proof line must have other proof lines specified
/// as antecedents, and the Justification checks whether the putative
/// consequent is supported by those antecedents based on that rule of
/// deduction. In the latter case the Assumption subclass will be used and no
/// antecedents should be specified.
/// 
/// Particular rules of deduction are represented as instances of a subclass; 
/// the subclasses themselves are broad categories of rules (e.g. DeMorgan's 
/// law identifies two logically equivalent forms, so it is an instance of
/// EquivalenceRule).
/// 
/// The specific rules of deduction that can be used in the proof are read
/// from the rules.xml file.
/// </summary>
class Justification
{
  private:
  char* rule_name;
  
  public:
  /// <summary>
  /// Construct a new justification rule.
  /// </summary>
  /// <param name="name">
  ///   The name of the rule, which is used to identify it in rules.xml and
  ///   proof input files. Also used when displaying the proof.
  /// </param>
  Justification(const char* name);
  virtual ~Justification();
  char* getName();
  
  /// <summary>
  /// Checks whether a statement is a logical consequence of certain antecedents
  /// based on this rule of deduction.
  /// </summary>
  /// <param name="consequent">
  ///   The proposed consequent of applying the rule of deduction
  /// </param>
  /// <param name="antecedents">
  ///   The antecedents on which the rule of deduction is applied.
  /// </param>
  /// <returns>
  ///   True if the proposed consequent is actually achieved by that 
  ///   application.
  /// </returns>
  virtual bool isJustified(StatementTree& consequent, 
     antecedent_list& antecedents) = 0;
  
  protected:

  /// <summary>
  /// Removes all entries from a bind map and frees their resources.
  /// </summary>
  /// <param name="binds">Bind map to clear</param>
  void removeBoundForms(bind_map& binds);

  /// <summary>
  /// Removes and frees all entries in one bind map which do not appear in
  /// another. This is used when a branch of searching for correspondance
  /// between proof lines and rule forms doesn't work out, and we need to
  /// backtrack and try a different one. Doesn't check whether the bound trees
  /// are equivalent, just whether there's an entry for a given proposition.
  /// </summary>
  /// <param name="new_binds">Map to remove forms from</param>
  /// <param name="old_binds">
  ///   Any form that's not in this map will be removed.
  /// </param>
  void removeNewlyBoundForms(bind_map& new_binds, bind_map& old_binds);
};

//TODO: This could be a singleton maybe?

/// <summary>
/// Subclass of Justification used for the premises of the proof, and the
/// assumption lines of any subproofs. An assumed line is considered to be
/// supported iff it does not claim to be based on any antecedents.
/// </summary>
class Assumption : public Justification
{
  public:
  /// <summary>
  /// Constructs the Assumption instance. Any instance of this class will have
  /// the name "Assumed".
  /// </summary>
  Assumption() : Justification("Assumed")
  {}
  
  /// <summary>
  /// A premise or assumption is always considered to be justified. It should
  /// not have any antecedents specified, so the only case this returns false
  /// is if it does.
  /// </summary>
  /// <param name="premise">
  ///   Premise or assumption statement. Contents are not relevant.
  /// </param>
  /// <param name="antecedents">
  ///   List of antecedents in the proof line, which should be empty.
  /// </param>
  /// <returns>True if there are no antecedents</returns>
  bool isJustified(StatementTree& premise, antecedent_list& antecedents)
  { return antecedents.size() == 0; }
};

#endif

