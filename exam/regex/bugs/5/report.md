# Bug 5: Null pointer dereference


## Description

When a regular expression is passed to the search engine, it will try to dereference its `match` method. If the regex is invalid (thus its addressis null), a null pointer will be dereferenced.


## Proof of Concept

(attached)


## Suggested Fix
Check that the regex given is non null before dereferencing it to get its `match` method. If it is the case, return -1 for a no match. 