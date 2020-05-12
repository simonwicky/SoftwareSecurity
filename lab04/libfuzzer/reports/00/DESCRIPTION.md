### Name
Double free

### Description
Chunk data is freed twice if `read_png_chunk()` fails. Once in `read_png_chunk()`, a second time in `load_png()`.

### Affected Lines
In `pngparser.c:301`and `pngparser.c:721`

### Expected vs Observed
We expect the programm to fail properly if a part is not valid, we observe a segmentation fault if `read_png_chunk()` fails.

### Steps to Reproduce
Run size with the poc provided as input

#### Command

```
./fuzzer_load_png crash-54632220f5c2cb61b58d4a50026e0c9a043685ea
```

### Suggested Fix Description
Set chunk->chunk_data to NULL after freeing it in `read_png_chunk()`.