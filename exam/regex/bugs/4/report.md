# Bug 4: Unsigned integer overflow


## Description

A Regular expression containing a range (type {n,m}) with a `m` bigger than `UINT_MAX` will cause the max variable to overflow in `parse.cpp:59`.


## Proof of Concept

(attached)


## Suggested Fix
Check for overflow and return an error if the range will overflow.