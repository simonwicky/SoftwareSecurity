# CS412: Final Project and Exam Replacement

This is the final exam/project for the course Software Security (CS412). The
project comprises three parts, loosely following the structure of the projects
you have already done during the semester.

- Manual code review, where you are expected to find bugs in code through
  inspection, exploit them and suggest fixes.
- Reverse engineering, where you are expected to understand x86 assembly and
  explain what it does, and the data-structures used.
- Fuzzing, where you fuzz a regex engine for bugs and report them.

The manual code review is for a protocol (GRASS) that provides remote access to a
regular-expression engine over the network. We provide one implementation of this
protocol which uses GNU's `grep` utility. An alternate approach would be to
write one's own regex engine. In fact, you shall fuzz one such engine in the final
part.

> Hint: Fuzzing campaigns take time, so you might want to start it before approaching
  the other parts.


## Scoring

The total score for this exam is 100 points distributed as follows:

- Manual code review: 50 points
  - 5 bugs, 10 points each
  - For each bug, 6 points for the report, 2 for the exploit, 2 for the fix.
  - The points breakdown for the report is described in the provided skeleton.
  - If a bug allows control-flow hijack or code-injection, and the provided
    exploit only causes a crash, we will award 1 point out of 2.
- Reverse engineering: 20 points
  - There are 5 reverse-engineering tasks, each awarding up to 4 points.
  - For each task, 2 points for the high-level description, 2 points for a
    corresponding C-code snippet.
- Fuzzing: 30 points
  - One libFuzzer stub, worth 5 points.
  - Setup and methodology description, worth 5 points
  - 5 bugs, worth 4 points per bug (2 points for description, 1 point for PoC,
    and 1 point for suggested fix).


## File structure

The file structure is as follows:

- `handout/` contains detailed descriptions of each part of the project.
- `grass/` contains the files for manual code review.
- `re/` contains the files for reverse engineering.
- `regex/` contains the files for fuzzing.


## Submission instructions

The project will be released on 19/06/2020 and is due at 23:59 on 28/06/2020.
As in previous projects, you will be expected to submit zipped files to Moodle
with the same structure as that provided to you.

For the manual code-review part, your work will be entirely contained in the
`grass/bugs/<n>` folders. Do not leave any other files lying around. Your
submission should also NOT contain the binaries (in `grass/bin`) generated
while compiling and running.

For the reverse engineering assignment, you are to submit five markdown files:
`re/X.md` where X is in 1..5. Each markdown file should contain a report on the
functionality of the corresponding shared library function. The format of
the report is described in the handout.

For the fuzzing part, you will be provided with the codebase in `regex/src`.
1. Modify the `fuzz.cpp` file to write your fuzzing stub.
1. Create a `regex/poc` directory in which you will save the crashing testcase
   for every bug you find, e.g., `regex/poc/BUG_1`.
1. Write your report (methodology and bug reports) into one file and save it in
   `regex/report.md`.

Do not leave compiled binaries or un-needed generated files
(e.g., fuzzer-specific seed files or coverage information) in the submitted
archive.

> Note: Ensure that your submitted archives **are not empty**. The
  submission deadline is final, and there will be no consideration for empty
  submissions.

> Note: EPFL's policy towards plagiarism will be strictly enforced. By
  submitting this project, you certify that you solved this project on
  your own, that the submission is your own, and that there were no
  environmental or other factors that disturbed you during this project
  or that diminished your performance.
