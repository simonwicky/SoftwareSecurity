# Report

## Stub design

Different design of stub were used and found different bugs.

### Fixed regex length
This stub splits the given data at a fixed position. The first 16 bytes are taken as regex, the rest is the text. The goal is to find basic bugs with small regex.  
This stub was tested first and found bug 1 and 2.  
The seed used with this stub was a basic regex of length 16 with an english random text. Some part of this text were modified to almost match the regex. Some part were modified to match the regex as well.

### Random regex length
This stub uses the first byte to determine the length of the regex. The whole data is split according to this length. Forst the regex, then the text. The goal is to find bugs that can trigger with longer regex.
This stub was tested second and found bug 3 and 4.
The seed used with this stub was a regex for email address. The first byte was set according to its length. Random text was following, with modified part to almost match the regex.

### Stub with no check on regex
The fixed regex length stub was also tested whithout error checkind on the regex vailidity, to catch bugs specifically in the searchEngine. This method helped to trigger bug 5.

## Sanitizers

### ASan
I used ASan to catch address related bugs like Segmentation Fault and double free. It was in use while bug 1 and 2 occured.

### UBSan
I used UBSan to catch various bugs, in particular unsigned integer overflow. UBSan was in use while bug 3 and 4 occured.

## Fuzzers
All bugs were found by libfuzzer although AFL has been tried for an extensive amount of time.  
The AFL wrapper for the stub has been left out, since it didn't catche any significant bugs.

## Setup
No particular setup has been used, except for the parameters already mentioned. With the good combination of sanitizers and stub, bugs all triggered very quickly. An extensive amount of time has nevertheless been spent on fuzzing time which did not ended in bug discovery.


## Stub organisation
The fixed regex length stub can be found in `fuzz_fixed.cpp`.  
The random regex length stub can be found in `fuzz_random.cpp`.
For convenience, both are also in `fuzz.cpp`, with one of them commented out.