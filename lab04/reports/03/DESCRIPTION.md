### Name
Attempt to dereference NULL pointer

### Description

If the palette is used, a plte_chunk must be present. No check is done to see if it is the case.

### Affected Lines
In `pngparser.c:528`.

### Expected vs Observed
We expect that a malformed YOLO PNG file causes the programm to report an error, we observe that it causes a segmentation fault while attempting to dereference a NULL pointer.

### Steps to Reproduce
Run the size programm with the afl-poc provided as input

#### Command

```
./size id:000003,sig:11,src:000000,op:flip1,pos:37
```

### Suggested Fix Description
Check that plte_chunk is non null when the palette is used.