### Name
Use after free

### Description

The struct pixel in `filter.c:101` is not dynamically allocated, hence freed at the end of the funtion `get_pixel`

### Affected Lines
In `filter.c:101`

### Expected vs Observed
We expect the negative filter to run correctly.
We observe a segmentation fault when trying to run the negative filter


### Steps to Reproduce
Filter a beforehand created png image using the negative filter.

#### Command

```
./filter in.png out.png negative
```

### Suggested Fix Description
Use `malloc` to allocate a `struct pixel` and `free`the pointer after use.