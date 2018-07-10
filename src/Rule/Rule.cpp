#include <Rule.h>
#include <vector>
#include <cstring>

#include <iostream>

constexpr uint8_t max_nb = 3 * 3 * 3 - 1;

BadRuleException::BadRuleException(const std::string& r)
    : std::runtime_error(r)
{

}

Rule::Rule()
{

}

Rule::Rule(std::string& _rule_str)
{
    set_rule(_rule_str);
}


Rule::~Rule()
{

}

uint32_t      Rule::get_min_survive()
{
    return the_rule[0];
}

uint32_t      Rule::get_max_survive()
{
    return the_rule[1];
}

uint32_t      Rule::get_min_born()
{
    return the_rule[2];
}

uint32_t      Rule::get_max_born()
{
    return the_rule[3];
}

std::string Rule::get_rule_str()
{
    return rule_str;
}

std::array<uint32_t, 4> Rule::get_rule()
{
    return the_rule;
}

void	    Rule::set_rule(std::string rule_str)
{
    std::vector<uint8_t> rule_values;
    std::string::size_type sz;
    if (rule_str.length())		rule_values.push_back(std::stoi(rule_str, &sz));
    if (rule_str.substr(sz).length())	rule_values.push_back(std::stoi(rule_str, &sz));
    if (rule_str.substr(sz).length())	rule_values.push_back(std::stoi(rule_str, &sz));
    if (rule_str.substr(sz).length())	rule_values.push_back(std::stoi(rule_str));

    if (rule_values.size() != 4){
	throw BadRuleException("Bad rule: size mismatch -> Rule string: " + rule_values[0]);
    }
    for (auto i = 0; i < 4; ++i){
	if (rule_values[i] > max_nb)
	    throw BadRuleException("Bad rule: value too large");
    }
    if (rule_values[0] > rule_values[1] || rule_values[2] > rule_values[3]){
	throw BadRuleException("Bad rule: min value larger than max value");
    }

    this->rule_str = rule_str;

    the_rule[0]	= rule_values[0];
    the_rule[1]	= rule_values[1];
    the_rule[2]	= rule_values[2];
    the_rule[3]	= rule_values[3];
}
