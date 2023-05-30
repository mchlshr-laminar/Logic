#ifndef __AGGREGATE_JUSTIFICATION_H_
#define __AGGREGATE_JUSTIFICATION_H_

#include "Justification.hpp"
#include <list>

//Stores a justification rule which has more than one possible format
//e.g. Addition can be a => a|b or a => b|a

/// <summary>
/// Used for justification rules which have more than one possible format. For
/// instance the rule named "Addition" can mean a => a|b or it can mean
/// a => b|a. An aggregate justification is applicable if any of its subforms
/// are.
/// </summary>
class AggregateJustification : public Justification
{
  private:
  std::list<Justification*> rules;
  
  public:
  AggregateJustification(const char* name) : Justification(name)
  {}
  virtual ~AggregateJustification();
  
  /// <summary>
  /// Checks whether this justification rule is applicable, which for an
  /// aggregate justification means at least one of the subrules is applicable.
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
  bool isJustified(StatementTree& consequent, antecedent_list& antecedents);
  
  /// <summary>
  /// Adds a possible form of this rule.
  /// </summary>
  /// <param name="new_rule">
  ///   New justification rule to try for applicablility.
  /// </param>
  void addRule(Justification* new_rule);
};

#endif

