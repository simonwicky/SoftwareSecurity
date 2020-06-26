# Task 3: Fuzzing (30 points)

In this part of the project, you will be exploring automatic bug detection
techniques by fuzzing a miniature [Regular Expressions](https://www.gnu.org/software/grep/manual/html_node/Fundamental-Structure.html)
engine. Throughout this exercise, you will collect crashes, triage them, and map
them back to their root cause, or bug. You will be expected to find **5 bugs**
in this code base, write bug reports describing the root cause, and suggest
fixes.

In short, the point distribution for this exercise is as follows:
* Writing a fuzzing stub: 5 points
* Setup and Methodology: 5 points
* Per bug: 4 points, distributed among
    * Description: 2 points
    * Proof of Concept: 1 point
    * Suggested fix: 1 point


## Regex: A summary

Regex is a technique developed to allow searching for occurrences of a pattern in
a text. As such, the regex engine in this code base is comprised of two
components:

* the parser
* the processor


### The Parser

The interface to access the parser is the function declared as:

```c++
RegularExpression * parse(const char * str, size_t len, int * err);
```

`str` is a character array of length `len`, representing the regex _pattern_.
`err` is a pointer to an int where the resulting error (0 == no error) is
stored.

The parser returns a `RegularExpression` object which can be fed to the search
component in order to scan a text for the first pattern match.


### The Processor

The interface to access the processor is the object whose constructor is
declared as:

```c++
SearchExpression(RegularExpression *regex);
```

`regex` is the pointer to the `RegularExpression` object created by the parser.
`SearchExpression` takes ownership of this object, and thus, it **should not**
be manually freed after construction.

The `SearchExpression` object exposes a function which performs the search:

```c++
std::tuple<int, int> SearchExpression::search(const char * str, unsigned int len);
```

`str` is the excerpt of _text_ in which to search for an occurrence of the regex
pattern, and `len` is its length. The `search` function returns an int tuple of
(index of match, length of match). These can be accessed as
`std::get<0>(result)` and `std::get<1>(result)`, respectively.


## The Fuzzing Process

As you've seen in a previous lab, libFuzzer can be a very handy tool for fuzzing
specific functions in libraries. In this project, you are first tasked with
writing a stub for fuzzing the regex engine.

A libFuzzer stub provides a random buffer and a length, from which you should
derive the inputs for fuzzing the regex parser and processor components. You are
free to conjure up any format or specification for deriving these inputs. For
instance, you could always consider the first 128 characters as the pattern
string, and the remaining characters as the text. You could also split the
buffer at a random point, or at the first occurrence of some character.
Different methods may have different effects on the effectiveness of your
fuzzing process, so you are encouraged to experiment. In the report, justify
the design of your fuzzer stub and your approach!

The regex engine was part of a student project and is thus prone to real bugs.
At least **5 bugs** have been verified to be found by a fuzzer, and you are
tasked with identifying and reproducing those bugs. The bugs will be counted as
valid as long as they are distinct (from one another) and they can be shown to
crash the engine.

**Note**: Pay attention not to introduce bugs as part of your fuzzing stub.
Ensure that the inputs to the interfaces are valid (i.e., the allocated buffer
size and the specified size are in agreement). Also ensure that you do not
introduce any use-after-free or double-free bugs, or memory leaks, when
constructing and destroying the inputs.


### Choice of Seeds

To kick start the fuzzing process, you could write a set of seeds that represent
a typical use of the regex engine, with valid regex expressions and matching
texts.

This practice is more likely to boost your fuzzing efficiency and produce more
compact crashing test cases, and you are encouraged to provide a seed corpus to
your fuzzer.


### Choice of Fuzzers

The stub you write will work with libFuzzer out-of-the-box, by adding the
`-fsanitize=fuzzer` flag during compilation.

However, that is not the only fuzzer that can be used. For instance,
[HonggFuzz](https://honggfuzz.dev/) has native support for compiling libFuzzer
stubs (no need to add `-fsanitize=fuzzer` when using its compiler). Moreover,
[AFL](https://lcamtuf.coredump.cx/afl/) can be used to fuzz libFuzzer stubs by
[*wrapping*](https://github.com/llvm-mirror/compiler-rt/blob/master/lib/fuzzer/standalone/StandaloneFuzzTargetMain.c)
the `LLVMFuzzerTestOneInput()` function inside a `main()` caller.

Different fuzzers employ different input generation strategies, and your choice
of fuzzer(s) could greatly impact how effectively you find the bugs.

**Hint**: If you get stuck in searching for a bug, try tuning fuzzer parameters.
At least one bug requires fairly large inputs (> 4096 bytes) to be triggered.

**Hint 2**: Disable ASLR on your machine when running the fuzzing campaigns.
This could make your results more reproducible.


### Choice of Sanitizers

AddressSanitizer is a versatile tool for detecting and trapping invalid memory
accesses. It can be very helpful in detecting accesses to out-of-bounds or
out-lived memory regions.

However, not all bugs in the engine are memory-safety bugs. You are encouraged
to explore [other sanitizers in the Clang compiler suite](https://clang.llvm.org/docs/index.html).
We suggest looking at least at ASan and UBsan.

**Note on UBSan**: By default, when using `-fsanitize=undefined`, not all
features of of UBSan are enabled. Some behaviors, such as unsigned integer
overflows, are not considered _undefined_ by the C standard, but could still be
bugs in the program. You are encouraged to manually enable such features to
increase the chance of catching such bugs. Check out the sanitizer-specific
documentation for more information on additional features.


### Fuzzing with Sanitizers

While ASan, by default, terminates the program after encountering an invalid
operation, not all sanitizers follow suit. However, for a fuzzer to mark an
invalid operation as a crash, the program must exit abruptly.

To enable such behavior, you can add one of `-fno-sanitize-recover=all` or
`-fsanitize-trap=all` to your build flags, in addition to `-fsanitize=...`.
Check the [Clang user manual](https://clang.llvm.org/docs/UsersManual.html), and
the specific sanitizer pages, for more information on these options. Experiment
with your fuzzer and sanitizers of choice to figure out which configuration
works best.


### Fuzz and Fix and Fuzz

The more crashes a fuzzer continuously encounters, the slower it becomes, and
the less useful are the crashing test-cases it generates. For this reason, you
are encouraged to triage and attempt to fix bugs as you encounter them. This
will speed up your fuzzing process as it only encounters crashes for the
remaining bugs.


## Building the Engine

To build your fuzz target, modify the `Makefile` and add compiler flags to
`EXTRA_FLAGS` according to your fuzzing configuration.

For instance, to enable libFuzzer with ASan, add `-fsanitize=fuzzer,address`.

Then run `make all` in the current directory. This will generate a `fuzz` target
at `build/bin/fuzz`.


## Your TODO: Fuzzing and Reporting

After performing the fuzzing process and finding the **5 bugs**, you should
write a report describing:

* The fuzzing methodology (choice of seeds/fuzzers/sanitizers)
  Carefully explain the design of your fuzzer stub and justify your
  design decisions. Similarly, discuss why you chose which sanitizers and
  what kind of starting seeds you used. Measure the amount of fuzzing time
  that was required to find bugs and what coverage you achieved.
* For every bug:
    * the type of bug and its implication
    * the sanitizer that detects it and how it detects it
    * the root cause in the code (along with a code snippet)
    * a possible fix (a brief description is sufficient)

In addition, you must provide the `fuzz.cpp` file which contains the stub, and
**a crashing test-case for every reported bug**.

You are not *required* to submit fixes for the bugs, but you are encouraged to
attempt to fix encountered bugs to improve your fuzzing process.


### Fuzzing Stub (5 points)

In `fuzz.cpp`, you will find a template for the `LLVMFuzzerTestOneInput()`
function, where you will consume fuzzer-generated input and feed it into the
different components of the regex engine.

It is important to note that all non-crashing execution paths (even paths which
encounter a handled parsing error) must terminate with `return 0;`. All other
return values for the exit code may be reserved by the fuzzer and should not be
used.


### Setup and Methodology (5 points)

In this section of the report, you should describe your setup and justify your:
* Design of the fuzzer stub
* Choice of seeds: what kinds of seeds you included and how they may have helped
  boost your fuzzing effectiveness
* Choice of sanitizers: which sanitizers helped find which types of bugs
* Choice of fuzzers: which fuzzer(s) you opted to use or try when searching for
  bugs
* Campaign setup: what fuzzing parameters you used (total duration, execution
  timeout, maximum file size, etc...). These parameters may be unique to each
  fuzzer and should be listed for every fuzzer used.


### Bug Reports (20 points)

Following the methodology description, you are tasked with writing a bug report
for every bug found. A sample report is listed here:

```markdown
# Bug 1: Out-of-bounds read on heap buffer


## Description (2 points)

There exists an out-of-bounds access to a heap-allocated buffer at
parse.cpp:123.

When the user provides a regex pattern with at least two nested capture groups
(a capture group is a regex expression surrounded by parantheses), the parser
mishandles the closing paranthesis of the inner group and consumes that of the
outer group. When the parser moves on to the closing paranthesis of the outer
group, it attempts to access data outside the bounds of the original pattern,
leading to a buffer over-read.


## Proof of Concept (1 point)

(attached)


## Suggested Fix (1 point)

When handling the closing paranthesis, make sure that no characters are consumed
needlessly. In our case, the parser inadvertently consumes the inner closing
paranthesis without processing them, then consumes the outer paranthesis as part
of processing the inner capture group. This is an off-by-one bug that can be
fixed by removing the additional `stmp++; ltmp--;` in handling the closing
parantheses.
```

**Note**: This is not a real bug in the code, but is rather just a demonstration
of the kind of bugs that could exist in such a program.
