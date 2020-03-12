### Name
Use after free

### Description
`resize.c:86` and `resize.c:87` both free the same pointer, which leads to an undefined behaviour.

### Affected Lines
In `resize.c:87`

### Expected vs Observed
We expect the programm to execute in a defined behaviour.
We observe that the programm crashes if it executes a `goto error_memory` or `goto error_memory_img` statement.

### Steps to Reproduce
None

### Suggested Fix Description
Remove the duplicate `free` statement.