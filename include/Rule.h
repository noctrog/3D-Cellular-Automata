#pragma once

#include <stdexcept>
#include <string>

class BadRuleException : public std::runtime_error
{
public:
    BadRuleException (const std::string& r);
};

class Rule
{
public:
    Rule (std::string& rule_str);
    virtual	~Rule ();

    uint8_t	get_min_survive();
    uint8_t	get_max_survive();
    uint8_t	get_min_born();
    uint8_t	get_max_born();
    
    std::string get_rule();
    void	set_rule(std::string rule_str);


private:
    std::string the_rule;
    uint8_t minSurvive;
    uint8_t maxSurvive;
    uint8_t minBorn;
    uint8_t maxBorn;
};
