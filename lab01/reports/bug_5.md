### Name
Write out of bounds

### Description
If the circle is too close to the edge of the image, it gets written out of bounds.

### Affected Lines
From `circle.c:49` to `circle.c:52`, from `circle.c:56` to `circle.c:59`, from `circle.c:70` to `circle.c:73` and from `circle.c:77` to `circle.c:80`

### Expected vs Observed
We expect the circle to be cropped if it is too close to the edge.
We observe a dubplicate circle of a segmentation fault if the radius is bigger than the distance between the center and the edge

### Steps to Reproduce
Create a circle where the radius is bigger than the center-edge distance.
#### Command
The image poc.png is 500x500
```
./circle poc.png poc2.png 500 500 100 ff0000   
```
### Suggested Fix Description
Check the bounds before writing