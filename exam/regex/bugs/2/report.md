# Bug 2: Assertion failed


## Description

A call to `RegularExpression::match_next` `(RegularExpression.cpp:29)` with incorrect parameters leads to an assertion failure and a crash.

An incorrect computaion of match_len in `LazyRepeatRangeExpression.cpp:29` leads to a call to `RegularExpression::match_next` with mlen > len.


## Proof of Concept

(attached)


## Suggested Fix
Correct `LazyRepeatRangeExpression.cpp:29` to remove the erroneous `+1`.