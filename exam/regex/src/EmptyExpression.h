#pragma once
#include "RegularExpression.h"
class EmptyExpression :
    public RegularExpression
{
protected:
    virtual int match_first(const char * str, unsigned int len) const;
    EmptyExpression(const EmptyExpression &re);

public:
    EmptyExpression();
    virtual RegularExpression * clone() const;
};