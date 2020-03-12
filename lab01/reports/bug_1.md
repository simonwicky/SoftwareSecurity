### Name
Command injection

### Description
A command can be injected while executing `solid` via the `output_name`

### Affected Lines
In `solid.c:122`

### Expected vs Observed
We expect the programm to display the size of the file created.
We observe that an arbitray command can be run.


### Steps to Reproduce
Generate a file called poc.png and generate an solid image named `poc.png\&\&whoami`

#### Command
```
./solid poc.png\&\&whoami 10 10 ffffff
```
### Suggested Fix Description
Avoid the use of `system()` and replace it by `stat()`from `sys/stat.h`