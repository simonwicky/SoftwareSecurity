### Name
Incorrect return value of `load_png()`

### Description

Load_png() returns 0 in case of success, and 1 in case of failure, which does not match its expected output from `size.c`.

### Affected Lines
In `pngparser.c:708` and `pngparser.c:725`.

### Expected vs Observed
We expect that a correct `load_png()` execution will lead to the rest of `size.c` being executed, and a wrong `load_png()` execution will stop the programm.  
We observe the opposite, leading to a segfault in case `load_png()` failed.

### Steps to Reproduce
Run size with the afl-poc provided as input

#### Command

```
./size id:000000,sig:11,src:000000,op:flip1,pos:0
```

### Suggested Fix Description
Swap the return value of `load_png()`, 1 for a success, 0 for an error. Alternatively, remove the `!`from `size.c:16` to invert the check.