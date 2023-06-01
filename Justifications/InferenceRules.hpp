#ifndef __INFERENCE_RULES_H_
#define __INFERENCE_RULES_H_

class InferenceRule;

#include "Justification.hpp"
#include "StatementTree.hpp"
#include "ProofStatement.hpp"
#include <utility>
#include <list>

/// <summary>
/// Stores the format which must be matched by one of a line's antecedents for
/// this rule to be applicable. If the rule requires a subproof, 
/// subproofAssumptionForm is the form that the assumption of that subproof
/// must have while statementForm is the form which must be found on some
/// line of the subproof. If the required form is not a subproof, 
/// subproofAssumptionForm will be null and statementForm has the form.
/// </summary>
struct required_form
{
  StatementTree* subproofAssumptionForm;
  StatementTree* statementForm;
};
typedef std::list<required_form*> required_form_list;

//Checks justification based on an inference rule

/// <summary>
/// For justification based on an inference rule. These rules may have
/// multiple required antecedents, and those antecedents may be required to be
/// subproofs which have a particular assumption and directly contain a
/// particular derived line. The forms specified in an inference rule must
/// match the entire sentence on a proof line, rather than any subsentence.
/// 
/// An example is Proof by Cases. To justify a sentence "c" by this rule, it
/// must have an antecedent line "a|b" and two subproof antecedents:
/// -One with the assumption "a" which contains a line "c"
/// -One with the assumption "b" which contains a line "c"
/// </summary>
class InferenceRule : public Justification
{
  private:
  StatementTree result_form;
  required_form_list required_forms;
  
  /// <summary>
  /// Attempts to match a target statement with a given required form while
  /// respecting any existing bindings between sentence variables in the form
  /// and statement trees.
  /// 
  /// If the form to match with is a sentence variable:
  ///   If that variable is unbound, match by binding that variable to the
  ///   sentence.
  ///   Otherwise a match can be made if the bound tree is the same as the
  ///   target statement.
  /// 
  /// If the form to match is not a sentence variable, a match can be made if
  ///   the node type and negation flag at the root of the required form 
  ///   matches the root of the target, AND the left and right subtrees of the
  ///   required form can match the left and right subtrees of the target.
  /// </summary>
  /// <param name="target">
  ///   Sentence to try to match with the required form, or a subtree of it
  ///   when called recursively.
  /// </param>
  /// <param name="form">
  ///   Required form to match, or a subtree of it when called recursively.
  /// </param>
  /// <param name="binds">
  ///   Contains any existing bindings between sentence variables and syntax
  ///   trees. If a match can be made, will be updated with any additional
  ///   bindings which are required to make that match.
  /// </param>
  /// <returns>True if a match can be made, false otherwise</returns>
  bool match(StatementTree* target, StatementTree* form, bind_map& binds);
  
  /// <summary>
  /// Attempts to match a required form and all subsequent required forms in
  /// the list with actual antecedents, while maintaining any existing bindings
  /// between sentence variables and syntax trees. Called on required_forms.begin()
  /// to match all required forms. When the end of the list is reached, checks
  /// that there are no antecedents that aren't matched to any required forms.
  /// 
  /// This will match the required form with the first antecedent it can, but if
  /// that doesn't allow all the subsequent forms to match then it will
  /// backtrack and match with a later antecedent.
  /// </summary>
  /// <param name="form">
  ///   Iterator into the list of required forms. Attempts to match that form,
  ///   then findAntecedentsForForms will be recursively called for the next
  ///   entry. If this is required_forms.end(), the function will just check
  ///   that statement_usage_map records a match for all antecedents.
  /// </param>
  /// <param name="ant">
  ///   The list of antecedents to match required forms with.
  /// </param>
  /// <param name="binds">
  ///   Stores bindings between sentence variables and syntax trees. For the
  ///   initial call this should the bindings to match the consequent form
  ///   with the proof line to be justified. On recursive calls, will also
  ///   contain any additional bindings for the previously matched forms.
  /// </param>
  /// <param name="ant_usage">
  ///   Maintains a count of how many required forms are matched to each
  ///   antecedent. If matching all required forms is successful, then this
  ///   will return true if this count is at least one for all antecedents.
  /// </param>
  /// <returns>
  ///   True if all required forms can be concurrently matched with antecedents
  ///   and all antecedents are matched with at least one required form.
  /// </returns>
  bool findAntecedentsForForms(required_form_list::iterator form, antecedent_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);

  /// <summary>
  /// Helper function for findAntecedentsForForms to match a required form
  /// which is not a subproof with an antecedent. Looks through antecedent_list
  /// for one which can match, then calls findAntecedentsForForms for the
  /// successor to the form iterator. If that recursive call succeeds, this has
  /// succeeded. If not, keep looking through the antecedents for another that
  /// matches. If none of them work out, matching has failed.
  /// </summary>
  /// <param name="form">
  ///   Iterator into the list of required forms, to the one we are trying to
  ///   match. Should not be the end iterator, and subproofAssumptionForm at
  ///   this value should be NULL.
  /// </param>
  /// <param name="ant">
  ///   The list of antecedents to match required forms with.
  /// </param>
  /// <param name="binds">
  ///   Stores bindings between sentence variables and syntax trees. Contains
  ///   the bindings to match the consequent and any previously matched
  ///   required forms. If matching is successful, will be updated with any
  ///   additional bindings for this and subsequent matchings.
  /// </param>
  /// <param name="ant_usage">
  ///   Maintains a count of how many required forms are matched to each
  ///   antecedent.
  /// </param>
  /// <returns>
  ///   True if this and each subsequent form can be matched with antecedents,
  ///   and all antecedents are used.
  /// </returns>
  bool findAntecedentsForBasicForm(required_form_list::iterator form, antecedent_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);

  /// <summary>
  /// Helper function for findAntecedentsForForms to match a required form
  /// which is a subproof with an antecedent. Looks through antecedent_list
  /// for one which:
  ///   -Is a subproof
  ///   -The subproof's assumption can match the required assumption form
  ///   -The subproof contains a child line which:
  ///     -Is not a subproof
  ///     -Can match with the required statement form.
  /// 
  /// When such a subproof is found, calls findAntecedentsForForms for the
  /// successor to the form iterator. If that recursive call succeeds, this has
  /// succeeded. If not, keep looking through the antecedents for another that
  /// matches. If none of them work out, matching has failed.
  /// </summary>
  /// <param name="form">
  ///   Iterator into the list of required forms, to the one we are trying to
  ///   match. Should not be the end iterator, and subproofAssumptionForm at
  ///   this value should NOT be NULL.
  /// </param>
  /// <param name="ant">
  ///   The list of antecedents to match required forms with.
  /// </param>
  /// <param name="binds">
  ///   Stores bindings between sentence variables and syntax trees. Contains
  ///   the bindings to match the consequent and any previously matched
  ///   required forms. If matching is successful, will be updated with any
  ///   additional bindings for this and subsequent matchings.
  /// </param>
  /// <param name="ant_usage">
  ///   Maintains a count of how many required forms are matched to each
  ///   antecedent.
  /// </param>
  /// <returns>
  ///   True if this and each subsequent form can be matched with antecedents,
  ///   and all antecedents are used.
  /// </returns>
  bool findAntecedentsForSubProof(required_form_list::iterator form, antecedent_list& ant,
    bind_map& binds, statement_usage_map& ant_usage);
  
  /// <summary>
  /// Helper function for findAntecedentsForForms, called once all required
  /// forms have been matched with antecedents. Checks that each listed
  /// antecedent has been matched with at least one required form.
  /// </summary>
  /// <param name="ant_usage">
  ///   Maintains a count of how many required forms are matched to each
  ///   antecedent.
  /// </param>
  /// <returns>True if all antecedents are used</returns>
  bool checkAntecedentRelevance(statement_usage_map& ant_usage);
  
  public:
  /// <summary>
  /// Constructs the inference rule. Required antecedent forms must be added
  /// after construction with addRequiredForm.
  /// </summary>
  /// <param name="result">The form the consequent must take</param>
  /// <param name="name">The name of the inference rule</param>
  InferenceRule(const char* result, const char* name) : Justification(name),
    result_form(result)
  {}
  
  virtual ~InferenceRule();
  
  /// <summary>
  /// Adds an antecedent form required to apply this rule.
  /// </summary>
  /// <param name="statement">
  ///   Form of the required antecedent sentence. If "assumption" is also
  ///   provided, this means the subproof antecedent must contain a derived
  ///   line with this form (which must be directly in the subproof, not any
  ///   sub-subproofs). Will be parsed into a syntax tree.
  /// </param>
  /// <param name="assumption">
  ///   Omit or set to NULL if the antecedent is not a subproof. If it is a
  ///   subproof, this is the form which the assumption of that subproof must
  ///   take. Will be parsed into a syntax tree.
  /// </param>
  void addRequiredForm(const char* statement, const char* assumption=NULL);
  
  /// <summary>
  /// A proof line is justified by this rule if that line matches the rule's
  /// consequent form, and for each required antecedent form in the rule the
  /// line has at least one listed antecedent that can match that form. All
  /// these matches must be consistent in terms of the bindings between 
  /// sentence variables and syntax trees. The line must also not have any
  /// extraneous antecedents which are not matched to any required form.
  /// </summary>
  /// <param name="con">
  ///   The proposed consequent of applying the rule.
  /// </param>
  /// <param name="ant">
  ///   The antecedents on which the rule will be applied.
  /// </param>
  /// <returns>
  ///   True if the proposed consequent is actually achieved by applying the
  ///   rule.
  /// </returns>
  bool isJustified(StatementTree& con, antecedent_list& ant);
};

#endif

