### Name
Read out of bounds

### Description

If the lenght of the data is not the same as advertised in the header chunk, the programm will read out of bounds of the inflated data. This is because the img struct uses uint8_t as size, while the header uses uint32.

### Affected Lines
In `pngparser.c:433-436` and `pngparser.c:481-485`.

### Expected vs Observed
We expect that a malformed YOLO PNG will cause the programm to return an error code, we observe that it can cause a segmentation fault.

### Steps to Reproduce
Run the fuzzer_load_png programm with the poc provided as input.

#### Command

```
./fuzzer_load_png crash-32ad13c1dbb3ba846b3c78f7876c16c005b82780
```

### Suggested Fix Description
Check for that the length of the inflated_buffer matches the advertised one. Also check that the advertised width and height is smaller than the inflated_buffer's size, to avoid overflow problem.