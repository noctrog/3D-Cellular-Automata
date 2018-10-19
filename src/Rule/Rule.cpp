#include <Rule.h>
#include <vector>
#include <cstring>

#include <iostream>
#include <sstream>

constexpr uint8_t max_nb = 3 * 3 * 3 - 1;

BadRuleException::BadRuleException(const std::string& r)
    : std::runtime_error(r)
{

}

Rule::Rule()
{

}

Rule::Rule(const std::string& _rule_str)
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

bool	      Rule::survives(size_t num_nb)
{
    if (num_nb >= get_min_survive() && num_nb <= get_max_survive())
    {
	return true;
    }
    else
    {
	return false;
    }
}

bool	      Rule::is_born(size_t num_nb)
{
    if (num_nb >= get_min_born() && num_nb <= get_max_born())
    {
	return true;
    }
    else
    {
	return false;
    }
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
    this->rule_str = rule_str;

    std::stringstream ss;
    ss.str(rule_str);
    ss >> the_rule[0] >> the_rule[1] >> the_rule[2] >> the_rule[3];

    //std::cout << the_rule[0] << " " << the_rule[1] << " " << the_rule[2] << " " << the_rule[3] << std::endl;
    
    if (the_rule[0] == 0 || the_rule[0] > max_nb ||
	the_rule[1] == 0 || the_rule[1] > max_nb ||
	the_rule[2] == 0 || the_rule[2] > max_nb ||
	the_rule[3] == 0 || the_rule[3] > max_nb){
	
	throw BadRuleException("Wrong rule values");
    }
}
