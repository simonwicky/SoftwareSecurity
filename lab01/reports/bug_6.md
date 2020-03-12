### Name
Buffer overflow

### Description
Buffers receiving argv input can be overflow due tuo the use of `strcpy`.

### Affected Lines
In `rect.c:20` and `rect.c:21`

### Expected vs Observed
We expect that the program runs correctly, even with large arguments.
We observe that the program crashes if the input or the output are longer than 256 character


### Steps to Reproduce
Try to generate a rect with a very long output name

#### Command
```
./rect poc.png aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.png 0 0 100 100 ffffff
```
### Suggested Fix Description
Use strncpy instead of strpy in the afor mentionned line