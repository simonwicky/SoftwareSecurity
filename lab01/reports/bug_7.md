### Name
Write out of bounds

### Description
`resize.c:35`and `resize.c:36` rounds with a simple cast, whereas `resize.c:54`and `resize.c:55` rounds with `round`. This can lead to an off-by-one error and a write out of bounds exception.

### Affected Lines
In `resize.c:66`

### Expected vs Observed
We expect the resize to execute properly with any factor > 0.
We observe that some factors < 1 lead to a segmentation fault.

### Steps to Reproduce
Create a picture of 500x500 and resize it using factor 0.333
#### Command
The image poc.png is 500x500
```
./resize_image poc.png poc2.png 0.333  
```
### Suggested Fix Description
Round everything using the same method, e.g. using the cast.