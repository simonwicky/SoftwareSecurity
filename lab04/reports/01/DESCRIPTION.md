### Name
Double free

### Description
Chunk data is freed twice if `read_png_chunk()` fails. Once in `read_png_chunk()`, a second time in `load_png()`.

### Affected Lines
In `pngparser.c:302`and `pngparser.c:719`

### Expected vs Observed
We expect the programm to fail properly if a part is not valid, we observe a segmentation fault if `read_png_chunk()` fails.

### Steps to Reproduce
Run size with the afl-poc provided as input

#### Command

```
./size id:000001,sig:11,src:000000,op:flip1,pos:8
```

### Suggested Fix Description
Set chunk->chunk_data to NULL after freeing it in `read_png_chunk()`.