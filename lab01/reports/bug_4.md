### Name
Buffer overflow

### Description
Buffers receiving argv input can be overflow due tuo the use of `strcpy`.

### Affected Lines
In `filter.c:177`,`filter.c:178`, `filter.c:179` and `filter.c:183`

### Expected vs Observed
We expect that the program runs correctly, even with large arguments.
We observe that the program crashes if the input, the output, the filter or the argument are longer than 256 character


### Steps to Reproduce
Try to filter an image with a very long argument, e.g. a very long output name

#### Command
```
./filter poc.png aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.png grayscale
```
### Suggested Fix Description
Use strncpy instead of strpy in the afor mentionned line
