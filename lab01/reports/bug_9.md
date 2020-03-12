### Name
Incorrect bound check for variable

### Description
A negative radius for the blur filter leads to blank image

### Affected Lines
In `filter.c:204`

### Expected vs Observed
We expect a negative radius will be treated as invalid and converted into a zero radius, i.e no change.
We observe that a negative radius leads to a blank image

### Steps to Reproduce
Filter an image with a negative radius
#### Command
```
./filter poc.png poc2.png blur -4   
```
### Suggested Fix Description
Test if the radius is negative, and convert it to zero if it is the case