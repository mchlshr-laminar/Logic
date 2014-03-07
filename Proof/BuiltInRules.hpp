#ifndef __BUILTINRULES_H_
#define __BUILTINRULES_H_

#define DEFAULT_RULES_FILENAME "rules.xml"
#define RULE_CHUNK_SIZE 20

#include "Proof.hpp"
#include "../rapidxml/rapidxml.hpp"
#include "../Justifications/Justification.hpp"
#include <map>

//The fact that clients can deallocate the justifications in the map is probably not ideal.
class BuiltInRules
{
	private:
	static justification_map rules;
	static bool rules_need_reading;
	
	static void readRulesFromFile();
	static Justification* readRuleNode(rapidxml::xml_node<>* rule_node);
	static Justification* readEquivalenceRule(rapidxml::xml_node<>* rule_node, char* rule_name);
	static Justification* readInferenceRule(rapidxml::xml_node<>* rule_node, char* rule_name);
	static Justification* readAggregateRule(rapidxml::xml_node<>* rule_node, char* rule_name);
	
	public:
	static const justification_map& getRuleMap();
};

#endif

/*
<equivalence rulename=DeMorgan>
	<pair form1="!a&!b" form2="!(a|b)" \>
<\equivalence>

<inference rulename="Modus Ponens" consequent=b>
	<antecedent form=a \>
	<antecedent form="a>b" \>
<\inference>
*/