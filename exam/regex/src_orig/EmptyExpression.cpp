#include "EmptyExpression.h"

EmptyExpression::EmptyExpression()
{
}

EmptyExpression::EmptyExpression(const EmptyExpression &re) :
    RegularExpression(re)
{
}

int EmptyExpression::match_first(const char * str, unsigned int len) const
{
	return 0;
}

RegularExpression * EmptyExpression::clone() const
{
    return new EmptyExpression(*this);
}