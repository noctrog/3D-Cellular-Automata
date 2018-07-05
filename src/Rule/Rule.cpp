#include <Rule.h>
#include <vector>
#include <cstring>

constexpr uint8_t max_nb = 3 * 3 * 3 - 1;

BadRuleException::BadRuleException(const std::string& r)
    : std::runtime_error(r)
{

}

Rule::Rule()
{

}

Rule::Rule(std::string& rule_str)
{
    set_rule(rule_str);
}


Rule::~Rule()
{

}

uint8_t	    Rule::get_min_survive()
{
    return minSurvive;
}

uint8_t	    Rule::get_max_survive()
{
    return maxSurvive;
}

uint8_t	    Rule::get_min_born()
{
    return minBorn;
}

uint8_t	    Rule::get_max_born()
{
    return maxBorn;
}

std::string Rule::get_rule()
{
    return the_rule;
}

void	    Rule::set_rule(std::string rule_str)
{
    std::vector<uint8_t> rule_values;

    for (auto i = std::strtok(&rule_str[0], " "); i != nullptr; i = std::strtok(NULL, " "))
    {
	rule_values.push_back(std::stoi(i));
    }

    if (rule_values.size() != 4){
	throw BadRuleException("Bad rule: size mismatch");
    }
    for (auto i = 0; i < 4; ++i){
	if (rule_values[i] > max_nb)
	    throw BadRuleException("Bad rule: value too large");
    }
    if (rule_values[0] > rule_values[1] || rule_values[2] > rule_values[3]){
	throw BadRuleException("Bad rule: min value larger than max value");
    }

    minSurvive	= rule_values[0];
    maxSurvive	= rule_values[1];
    minBorn	= rule_values[2];
    maxBorn	= rule_values[3];
}
