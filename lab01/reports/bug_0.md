### Name
Incorrect bound check

### Description

The height and width of the picture are supposed to be greater than 0, but this checks is performed wrongly and it is possible to generate a corrupted PNG image.

### Affected Lines
In `solid.c:47`,`solid.c:54`, `checkerboard.c:38` and `checkerboard.c:45`.

### Expected vs Observed
We expect that a negative or 0 width or height yield an error. Instead, we observe that inputting a 0 width or height create a corrupted PNG image.

### Steps to Reproduce
Create a solid image with a 0 width or height with the command below

#### Command

```
./solid poc.png 0 0 ffffff
```

### Suggested Fix Description
Correct the bound check `width < 0` and `height < 0` by `width <= 0` respectively `height <= 0`