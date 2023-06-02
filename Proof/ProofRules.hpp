#ifndef __PROOFRULES_H_
#define __PROOFRULES_H_

#define DEFAULT_RULES_FILENAME "rules.xml"
#define RULE_CHUNK_SIZE 20

#include "rapidxml.hpp"
#include "Justification.hpp"
#include <map>
#include <string>

typedef std::map<std::string, Justification*> justification_map;

//TODO: In the future it might be good to make the use of justifications const

/// <summary>
/// Static class which creates, stores, and retrieves the justification rules
/// usable in the proof. Default rules are read from the DEFAULT_RULES_FILENAME
/// file (probably rules.xml) and additional rules can be added by the proof
/// using lemmas.
/// 
/// Rules are retrieved by name, and the rules file will be read the first 
/// time a rule is retrieved.
/// 
/// Uses rapidxml to parse the XML rules file.
/// </summary>
class ProofRules
{
  private:
  static justification_map rules;
  static bool rules_need_reading;
	
  /// <summary>
  /// Reads the rules file and stores the rules in the map. This will be called
  /// the first time findRule or addRule is called.
  /// </summary>
  static void readRulesFromFile();

  /// <summary>
  /// Helper for readRulesFromFile. Parses the XML node for one rule and
  /// constructs a Justification object for it. If the node has no "rulename"
  /// attribute defined, the created rule will have the name "Unnamed Rule".
  /// </summary>
  /// <param name="rule_node">XML node for the rule</param>
  /// <returns>Justification constructed from the node</returns>
  static Justification* readRuleNode(rapidxml::xml_node<>* rule_node);

  /// <summary>
  /// Helper for readRuleNode, for when the rule node type is "equivalence".
  /// Constructs an EquivalenceRule from the equivalent pairs which should be
  /// in "pair" nodes that are children of the rule node.
  /// </summary>
  /// <param name="rule_node">XML node for the rule</param>
  /// <param name="rule_name">
  ///	Name of the rule from the "rulename" attribute, or "Unnamed Rule" if
  ///	there was none.
  /// </param>
  /// <returns>EquivalenceRule</returns>
  static Justification* readEquivalenceRule(rapidxml::xml_node<>* rule_node, char* rule_name);

  /// <summary>
  /// Helper for readRuleNode, for when the rule node type is "inference".
  /// Constructs an InferenceRule, with the consequent form from the 
  /// "consequent" attribute of the rule node, and antecedent forms from its
  /// "antecedent" child nodes.
  /// </summary>
  /// <param name="rule_node">XML node for the rule</param>
  /// <param name="rule_name">
  ///	Name of the rule from the "rulename" attribute, or "Unnamed Rule" if
  ///	there was none.
  /// </param>
  /// <returns>InferenceRule</returns>
  static Justification* readInferenceRule(rapidxml::xml_node<>* rule_node, char* rule_name);

  /// <summary>
  /// Helper for readRuleNode, for when the rule node type is "aggregate".
  /// Calls readRuleNode for each of the rule node's children and assembles them
  /// into an AggregateJustification.
  /// </summary>
  /// <param name="rule_node">XML node for the rule</param>
  /// <param name="rule_name">
  ///	Name of the rule from the "rulename" attribute, or "Unnamed Rule" if
  ///	there was none.
  /// </param>
  /// <returns>AggregateJustification</returns>
  static Justification* readAggregateRule(rapidxml::xml_node<>* rule_node, char* rule_name);
	
  public:

  /// <summary>
  /// Finds a rule by name. If the rules file has not been read yet, will load
  /// the rules (with readRulesFromFile).
  /// </summary>
  /// <param name="rule_name">The name of the rule to retrieve</param>
  /// <returns>
  ///	The justification rule of that name. If there is no justification by
  ///	that name, returns null.
  /// </returns>
  static Justification* findRule(const char* rule_name);

  /// <summary>
  /// Adds a new rule to the map, which can be used in the proof. These should
  /// be added by way of a lemma command in the proof. If the justification is
  /// null, doesn't have a valid name, or its name is already used in the map,
  /// then the rule will not be added.
  /// 
  /// If the rules file has not been read yet, will load the rules.
  /// </summary>
  /// <param name="added_rule">New justification rule to add</param>
  /// <returns>True if the rule was added</returns>
  static bool addRule(Justification* added_rule);
};

#endif
