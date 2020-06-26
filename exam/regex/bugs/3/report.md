# Bug 3: Unsigned integer overflow


## Description

A Regular expression containing a range (type {n,m}) with a n bigger than UINT_MAX will cause the min variable to overflow in `parse.cpp:45`.


## Proof of Concept

(attached)


## Suggested Fix
Check for overflow and return an error if the range will overflow