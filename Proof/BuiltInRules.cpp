#include "BuiltInRules.hpp"
#include "../Justifications/InferenceRules.hpp"
#include "../Justifications/EquivalenceRules.hpp"
#include "../Justifications/AggregateJustification.hpp"
#include <cstring>
#include <string>
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

bool BuiltInRules::rules_need_reading = true;
justification_map BuiltInRules::rules;

//Creates the rule map from the XML input file.
void BuiltInRules::readRulesFromFile()
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
		//TODO: if(read_size == -1) error
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

//Translates one XML node into a rule in the map.
Justification* BuiltInRules::readRuleNode(xml_node<>* rule_node)
{
	char* rule_name = NULL;
	xml_attribute<>* attr = rule_node->first_attribute("rulename");
	if(attr == NULL)
	{
		rule_name = new char[13];
		strcpy(rule_name, "Unnamed Rule");
	}
	else
	{
		rule_name = new char[attr->value_size()+1];
		strcpy(rule_name, attr->value());
	}
	
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

Justification* BuiltInRules::readEquivalenceRule(xml_node<>* rule_node, char* rule_name)
{
	bool has_added_pairs = false;
	EquivalenceRule* created_rule = new EquivalenceRule(rule_name);
	
	for(xml_node<>* pair_node = rule_node->first_node("pair"); pair_node != NULL; pair_node = pair_node->next_sibling("pair"))
	{
		xml_attribute<>* first_form = pair_node->first_attribute("form1");
		xml_attribute<>* second_form = pair_node->first_attribute("form2");
		if(first_form == NULL || second_form == NULL) continue;
		if(first_form->value_size() <= 0 || second_form->value_size() <= 0) continue;
		
		created_rule->addEquivalentPair(first_form->value(), second_form->value());
		has_added_pairs = true;
	}
	
	if(!has_added_pairs)
	{
		delete created_rule;
		return NULL;
	}
	return (Justification*)created_rule;
}

Justification* BuiltInRules::readInferenceRule(xml_node<>* rule_node, char* rule_name)
{
	xml_attribute<>* consequent = rule_node->first_attribute("consequent");
	if(consequent == NULL || consequent->value_size() <= 0) return NULL;
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

Justification* BuiltInRules::readAggregateRule(xml_node<>* rule_node, char* rule_name)
{
	bool has_added_subrules = false;
	AggregateJustification* created_rule = new AggregateJustification(rule_name);
	
	for(xml_node<>* subrule_node = rule_node->first_node(0); subrule_node != NULL; subrule_node = subrule_node->next_sibling(0))
	{
		Justification* subrule = readRuleNode(subrule_node);
		
		if(subrule != NULL)
		{
			created_rule->addRule(subrule);
			has_added_subrules = true;
		}
	}
	
	if(!has_added_subrules)
	{
		delete created_rule;
		return NULL;
	}
	return (Justification*)created_rule;
}

const justification_map& BuiltInRules::getRuleMap()
{
	if(rules_need_reading) readRulesFromFile();
	return rules;
}
