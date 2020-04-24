# Answers to the questions 

### 1. Why did you need to change `is_png_chunk_valid`?
`is_png_chunk_valid()` performs a crc check. This check can fail and the input causing this crash will not be tested with the actual functions in the library.


### 2. Why did we give you exactly TWO seeds for fuzzing?
The library allows two type of PNG. One with a palettre, the other one with RGBA value, hence 2 seeds.

### 3. Why did you have to use `afl-gcc` to compile the source (and not e.g. ordinary gcc)?
We need afl-gcc to instrument the code, so it can more smartly, in a white box fuzzing manner.

### 4. How many crashes in total did AFL produce? How many unique crashes?
Using AFL in a docker running Ubuntu for 1 minutes, it found 16.1k crashes, for 23 unique ones.

### 5. Why are hangs counted as bugs in AFL? Which type of attack can they be used for?
None. We could use hangs to mount a DoS attack against a machine.

### 6. Which interface of `libpngparser` remains untested by AFL (take a look at `pngparser.h`)?
`store_png()` was not tested
### 7. In lab0x01 many students encountered a crash in `load_png` when the input file with the specified name didn't exist. Can AFL find that bug in this setup?
No, since we tell AFL to use existing file, and to leverage them to find crashes.

### 8. How long did you run AFL for? If you run it for twice as long, do you expect to find twice as many bugs? Why?
I ran AFL for 1 minutes. Although the increase of crashes seems linear, it should eventually stops. Hence for short duration, we can expect AFL to find twice as many crashes for an amount of time twice as long.  
This does not hold for unique crashes.
