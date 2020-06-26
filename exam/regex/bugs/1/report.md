# Bug 1: Use after free on the heap


## Description

There exist a use after free in `parse.cpp:239`, when the regex pattern is invalid and the parser errors.

When a regex is invalid, the parser frees the whole list of regex, then tries to free `next`. If `next` was already added to the list, it introduces a double free.


## Proof of Concept

(attached)


## Suggested Fix
When `next` is added to the list in `parse.cpp:219`, set it to `nullptr`.If it then errors, it won't be freed again, if it does not error, it will be reinitialized.