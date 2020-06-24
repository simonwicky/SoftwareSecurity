#include "re.h"
#include <stdint.h>
#include <string.h>

#define REG_LEN 128

extern "C"
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    /**
     * The regex engine is comprised of two components:
     * - the parser
     * - the processor
     * In this stub, you are expected to fuzz both components simultaneously,
     *   in order to keep the implementation simple.
     *
     * The interface to access the parser is the function declared as:
     *   RegularExpression * parse(const char * str, size_t len, int * err);
     * `str` is a character array of length `len`. `err` is a pointer to an int
     *   where the resulting error (0 == no error) is stored.
     * The parser returns a `RegularExpression` object which can be fed to the
     *   search component in order to scan a text for the first pattern match.
     *
     * The interface to access the processor is the object declared as:
     *   SearchExpression(RegularExpression *regex);
     *   std::tuple<int, int> SearchExpression::search(const char * str, unsigned int len);
     * `regex` is the pointer to the RegularExpression object created by the
     *   parser. `SearchExpression` takes ownership of this object, and thus, it
     *   should not be manually free'd after construction.
     * `str` is the excerpt of text in which to search for an occurence of the
     *   regex pattern, and `len` is its length. The `search` function returns
     *   an int tuple of (index of match, length of match). These can be
     *   accessed as std::get<0>(result) and std::get<1>(result), respectively.
     * For the purposes of this fuzzer, we are not interested in the result, but
     *   rather in the input validation and error-handling of the regex engine.
     *
     * Write your libFuzzer stub here, where you first split the input `Data`
     *   buffer into two strings, `pattern` and `text`, then feed those inputs
     *   into the corresponding interfaces.
     * It remains up to you how to split the input to maximize the chance of
     *   finding bugs.
     *
     * All non-crashing execution paths must terminate with `return 0;`. All
     *   other return values may be reserved by the fuzzer.
     *
     * Triage and attempt to fix bugs as you encounter them. This will speed up
     *   your fuzzing process as it only encounters crashes for the remaining
     *   bugs.
     */
//     if (Size == 0) {
//          return 0;
//     }
//     uint8_t *length = (uint8_t *)Data;

//      if(Size < *length + 1) {
//           return 0;
//      }

//      char regex[*length + 1];
//      memcpy(regex, &Data[1], *length);
//      regex[*length] = '\0';

//      char text[Size - *length];
//      memcpy(text, &Data[*length + 1], Size - *length - 1);
//      text[Size - *length - 1] = '\0';

//      int err = 0;
//      RegularExpression *r = parse(regex, strlen(regex), &err);
//      if(err != 0) {
//           return 0;
//      }
//      SearchExpression s(r);
//      s.search(text, strlen(text));
//      return 0;
     if(Size < REG_LEN) {
          return 0;
     }

     char regex[REG_LEN + 1];
     memcpy(regex, Data, REG_LEN);
     regex[REG_LEN] = '\0';

     char text[Size - REG_LEN + 1];
     memcpy(text, &Data[REG_LEN], Size - REG_LEN);
     text[Size - REG_LEN] = '\0';

     int err = 0;
     RegularExpression *r = parse(regex, strlen(regex), &err);
     if(err != 0) {
          return 0;
     }
     SearchExpression s(r);
     s.search(text, strlen(text));
     return 0;

}

// int main(int argc, char *argv[]){

//      FILE* f = fopen(argv[1],"r");
//      size_t size;
//      fseek(f, 0L, SEEK_END);
//      size = ftell(f);
//      fseek(f, 0L, SEEK_SET);
//      uint8_t buf[size];
//      fread(buf, sizeof(uint8_t), size, f);
//      fclose(f);

//      buf[size] = 0;

//      return LLVMFuzzerTestOneInput(buf, size);
// }