#include "AlternationExpression.h"

AlternationExpression::AlternationExpression(const RegularExpression * const first, const RegularExpression * const rest)
    : first(first), rest(rest)
{
}

AlternationExpression::AlternationExpression(const AlternationExpression &re) :
    RegularExpression(re),
    first(re.first->clone()),
    rest(re.rest->clone())
{
}

AlternationExpression::~AlternationExpression()
{
    if (first != nullptr) {
        delete first;
    }
    if (rest != nullptr) {
        delete rest;
    }
}

int AlternationExpression::match_first(const char * str, unsigned int len) const
{
    int mlen = first->match(str, len);
    if (mlen < 0) {
        return rest->match(str, len);
    } else {
        return mlen;
    }
}

RegularExpression * AlternationExpression::clone() const
{
    return new AlternationExpression(*this);
}