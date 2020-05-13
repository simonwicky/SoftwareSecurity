### Name
Attempt to dereference NULL pointer

### Description

If the palette is used, a plte_chunk must be present. No check is done to see if it is the case.

### Affected Lines
In `pngparser.c:417`.

### Expected vs Observed
We expect that a malformed YOLO PNG file causes the programm to report an error, we observe that it causes a segmentation fault while attempting to dereference a NULL pointer.

### Steps to Reproduce
Run the fuzzer_load_png programm with the poc provided as input

#### Command

```
./fuzzer_load_png crash-8586ea3f15c61a9320070f73d851850ad3099132
```

### Suggested Fix Description
Check that plte_chunk is non null when the palette is used.