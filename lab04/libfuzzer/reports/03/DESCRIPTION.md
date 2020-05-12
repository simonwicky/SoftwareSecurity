### Name
Memory leak

### Description

In case of error while deflating data the iend_chunk in not freed.

### Affected Lines
Allocated in `pngparser.c:589`.

### Expected vs Observed
We expect no memory leaks, but we get one.

### Steps to Reproduce
Run the fuzzer_load_png programm with the poc provided as input.

#### Command

```
./fuzzer_load_png crash-32ad13c1dbb3ba846b3c78f7876c16c005b82780
```

### Suggested Fix Description
Free the chunk in case of error