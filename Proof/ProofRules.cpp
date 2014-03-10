#include "Proof/ProofRules.hpp"
#include "Justifications/InferenceRules.hpp"
#include "Justifications/EquivalenceRules.hpp"
#include "Justifications/AggregateJustification.hpp"
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

using std::stringstream;
using std::string;
using std::cerr;
using std::endl;
using rapidxml::xml_document;
using rapidxml::xml_node;
using rapidxml::xml_attribute;

bool ProofRules::rules_need_reading = true;
justification_map ProofRules::rules;

//Creates the initial rule map from the XML input file.
void ProofRules::readRulesFromFile()
{
	//Open and read file to a stringstream.
	stringstream input_string;
	char* input_buffer = new char[RULE_CHUNK_SIZE+1];
	int fd = open(DEFAULT_RULES_FILENAME, O_RDONLY);
	if(fd == -1)
	{
		cerr << "Error: rules file " << DEFAULT_RULES_FILENAME << " could not be opened." << endl;
		exit(1);
	}
	
	int read_size;
	while( (read_size = read(fd, input_buffer, RULE_CHUNK_SIZE)) )
	{
		if(read_size == -1)
		{
			cerr << "Error: rules file was opened but couldn't be read." << endl;
			exit(1);
		}
		input_buffer[read_size] = '\0';
		input_string << input_buffer;
	}
	close(fd);
	
	//rapidxml modifies the c string it parses; copy data to a non-const c-string, then parse.
	delete [] input_buffer;
	input_buffer = new char[input_string.str().size()+1];
	strcpy(input_buffer, input_string.str().c_str());
	xml_document<> input_structure;
	try
	{
    input_structure.parse<0>(input_buffer);
	}
	catch(int e)
	{
		cerr << "Error: rules file could not be parsed, exception id " << e << endl;
		exit(2);
	}
	
	//Create rules from nodes
	for(xml_node<>* rule_node = input_structure.first_node(0); rule_node != NULL; rule_node = rule_node->next_sibling(0))
	{
		Justification* new_rule = readRuleNode(rule_node);
		if(new_rule == NULL) continue;
		
		xml_attribute<>* rule_name = rule_node->first_attribute("rulename");
		if(rule_name == NULL)
		{
			//TODO: Error or generate name?
			delete new_rule;
		}
		else
		{
			rules[string(rule_name->value())]=new_rule;
		}
	}

	rules_need_reading = false;
	input_structure.clear();
	delete [] input_buffer;
}

//Translates one XML node into a Justification object.
Justification* ProofRules::readRuleNode(xml_node<>* rule_node)
{
  //Read the rule name
	char* rule_name = NULL;
	xml_attribute<>* attr = rule_node->first_attribute("rulename");
	if(attr == NULL)
	{
    //Aggregate justification rules have unnamed sub-rules, so this isn't an error condition.
		rule_name = new char[13];
		strcpy(rule_name, "Unnamed Rule");
	}
	else
	{
		rule_name = new char[attr->value_size()+1];
		strcpy(rule_name, attr->value());
	}
	
  //Read the rest of the rule.
	Justification* retval = NULL;
	if(strcmp(rule_node->name(), "equivalence") == 0)
		retval = readEquivalenceRule(rule_node, rule_name);
	else if(strcmp(rule_node->name(), "inference") == 0)
		retval = readInferenceRule(rule_node, rule_name);
	else if(strcmp(rule_node->name(), "aggregate") == 0)
		retval = readAggregateRule(rule_node, rule_name);
	delete [] rule_name;
	return retval;
}

//Creates an EquivalenceRule object from an appropriate XML node, returns it as a Justification.
//Is a helper for readRuleNode.
Justification* ProofRules::readEquivalenceRule(xml_node<>* rule_node, char* rule_name)
{
	bool has_added_pairs = false;
	EquivalenceRule* created_rule = new EquivalenceRule(rule_name);
	
  //Read all the equivalent pairs
	for(xml_node<>* pair_node = rule_node->first_node("pair"); pair_node != NULL; pair_node = pair_node->next_sibling("pair"))
	{
		xml_attribute<>* first_form = pair_node->first_attribute("form1");
		xml_attribute<>* second_form = pair_node->first_attribute("form2");
		if(first_form == NULL || second_form == NULL) continue;
		if(first_form->value_size() <= 0 || second_form->value_size() <= 0) continue;
		
		created_rule->addEquivalentPair(first_form->value(), second_form->value());
		has_added_pairs = true;
	}
	
  //An equivalence rule must have at least on equivalent pair.
	if(!has_added_pairs)
	{
    cerr << "Error in rules file: equivalence " << rule_name << " is empty and cannot be created.\n";
		delete created_rule;
		return NULL;
	}
	return (Justification*)created_rule;
}

//Creates an InferenceRule object from an appropriate XML node, returns it as a Justification.
//Is a helper for readRuleNode.
Justification* ProofRules::readInferenceRule(xml_node<>* rule_node, char* rule_name)
{
	xml_attribute<>* consequent = rule_node->first_attribute("consequent");
  //An inference rule must have a consequent form, otherwise it does nothing.
	if(consequent == NULL || consequent->value_size() <= 0)
  {
    cerr << "Error in rules file: " << rule_name << " has no consequent and cannot be created.\n";
    return NULL;
  }
	InferenceRule* created_rule = new InferenceRule(consequent->value(), rule_name);
	
	xml_node<>* ant_node = rule_node->first_node("antecedent");
	for(; ant_node != NULL; ant_node = ant_node->next_sibling("antecedent"))
	{
		//It is OK for an inference rule to have no antecedents.
		xml_attribute<>* ant_form = ant_node->first_attribute("form");
		xml_attribute<>* assumption_form = ant_node->first_attribute("subproof");
		if(ant_form == NULL || ant_form->value_size() <= 0) continue;
		
		if(assumption_form == NULL || assumption_form->value_size() <= 0)
			created_rule->addRequiredForm(ant_form->value());
		else
			created_rule->addRequiredForm(ant_form->value(), assumption_form->value());
	}
	
	return (Justification*)created_rule;
}

//Creates an AggregateJustification object from an appropriate XML node, returns it as a Justification.
//Is a helper for readRuleNode; readRuleNode is called recursively to read the sub-rules.
Justification* ProofRules::readAggregateRule(xml_node<>* rule_node, char* rule_name)
{
	bool has_added_subrules = false;
	AggregateJustification* created_rule = new AggregateJustification(rule_name);
	
  //Read each sub-rule
	for(xml_node<>* subrule_node = rule_node->first_node(0); subrule_node != NULL; subrule_node = subrule_node->next_sibling(0))
	{
		Justification* subrule = readRuleNode(subrule_node);
		
		if(subrule != NULL)
		{
			created_rule->addRule(subrule);
			has_added_subrules = true;
		}
	}
	
  //Must have at least one sub-rule (hopefully more than one though, or WTF are you doing)
	if(!has_added_subrules)
	{
		delete created_rule;
		return NULL;
	}
	return (Justification*)created_rule;
}

//Finds and returns the justification rule with the given name. Returns NULL if no such rule is found.
//Reads default rules from the file first if that has not yet happened.
Justification* ProofRules::findRule(const char* rule_name)
{
  if(rules_need_reading) readRulesFromFile();
  //If a user lemma is defined before any justification rules have been used (including in lemma proofs) then
  //this could be a problem. I don't think that's possible though; investigate.
  //Something else using addRule could be problematic though.
  
  justification_map::iterator itr = rules.find(string(rule_name));
  if(itr == rules.end()) return NULL;
  return itr->second;
}

//Adds the given rule to the map if no rule with the same name already exists. Returns whether the
//rule was successfully added.
bool ProofRules::addRule(Justification* added_rule)
{
  if(added_rule == NULL) return false;
  
  char* rule_name = added_rule->getName();
  if(rule_name == NULL || strcmp(rule_name, "") == 0 || rules.find(string(rule_name)) != rules.end())
    return false;
  
  rules[string(rule_name)] = added_rule;
  return true;
}
