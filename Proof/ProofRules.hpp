#ifndef __PROOFRULES_H_
#define __PROOFRULES_H_

#define DEFAULT_RULES_FILENAME "rules.xml"
#define RULE_CHUNK_SIZE 20

#include "../rapidxml/rapidxml.hpp"
#include "../Justifications/Justification.hpp"
#include <map>
#include <string>

class StringMapper
{
  public:
  bool operator()(std::string str1, std::string str2)
  { return str1.compare(str2) < 0; }
};

typedef std::map<std::string, Justification*, StringMapper> justification_map;

//In the future it might be good to make the use of justifications const

//Creates, stores, and retrieves the justification rules usable in the proof. Default rules are read
//from the file DEFAULT_RULES_FILENAME and additional rules can be added by the user
//(using lemmas presumably).
class ProofRules
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
	//static const justification_map& getRuleMap();
  static Justification* findRule(const char* rule_name);
  static bool addRule(Justification* added_rule);
};

#endif
