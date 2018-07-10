#pragma once

#include <stdexcept>
#include <string>
#include <array>

class BadRuleException : public std::runtime_error
{
public:
    BadRuleException (const std::string& r);
};

class Rule
{
public:
    Rule ();
    Rule (std::string& rule_str);
    virtual		    ~Rule ();

    uint32_t		    get_min_survive();
    uint32_t		    get_max_survive();
    uint32_t		    get_min_born();
    uint32_t		    get_max_born();
    
    std::array<uint32_t, 4> get_rule();
    std::string		    get_rule_str();
    void		    set_rule(std::string rule_str);


private:
    std::string rule_str;
    std::array<uint32_t, 4> the_rule;
};
