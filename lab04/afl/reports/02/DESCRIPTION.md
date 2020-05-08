### Name
Read out of bounds

### Description

If the lenght of the data is not the same as advertised in the header chunk, the programm will read out of bounds of the inflated data.

### Affected Lines
In `pngparser.c:439-439` and `pngparser.c:484-487`.

### Expected vs Observed
We expect that a malformed YOLO PNG will cause the programm to return an error code, we observe that it can cause a segmentation fault.

### Steps to Reproduce
Run the size programm with the afl-poc provided as input.

#### Command

```
./size id:000000,sig:11,src:000000,op:flip1,pos:16
```

### Suggested Fix Description
Check for that the length of the inflated_buffer matches the advertised one. Also check that the advertised width and height is smaller than the inflated_buffer's size, to avoid overflow problem.