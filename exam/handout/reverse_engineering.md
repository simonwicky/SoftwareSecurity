# Task 2: Reverse Engineering (20 points)

Assignment 2 consists of 5 shared libraries (`task1-5.so`). You are to reverse
engineer the function exported by each of the shared libraries. All functions
have the same name as the library containing them.

Every task is worth four (4) points. Two (2) points will be awarded for the
correct C code of the function. Another two (2) will be awarded for the correct
high-level description of the function's behavior.

You will have to provide the definitions of all the data types used by the
functions. Use `char*` for strings. Numeric types should be named after their width:
`uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`, ...


You are free to use Ghidra or IDA Pro.

# Your TODO: Reversing and Reporting

The RE directory should contain 5 Markdown files (`1-5.md`) each containing a
report on one of the tasks. Reports consist of two sections:

- code
- description

The code section contains the C source code of the reversed function from the
library (`taskX` from `taskX.so`). Besides the function, the code should also
include the definitions of all the structures used. Rename the variables,
functions and types to reflect their actual purpose.

The description provides an overview of the function. It discusses the
function's inputs, outputs and purpose. For the types describe the significance
of each field. Try to be as clear and concise as possible. We value conciseness
over length!


# Example Report 1


## Code

```
uint32_t factorial(uint32_t n)
{
    if (!n) return 1;

    return n * factorial(n-1);
}

```

## Description

The function calculates the factorial function recursively. It takes a 32-bit
unsigned integer as the argument and returns a 32-bit unsigned integer as the
result.


# Example Report 2


## Code

```
struct ll_node
{
    int32_t val;
    struct ll_node *next;
}

int32_t sum(struct ll_node *root)
{
    int32_t result = 0;

    while (root)
    {
        result += root->val;
        root = root->next;
    }

    return result;
}
```


## Description

The function calculates the sum of all elements in a linked list. The head of
the list `(struct ll_node)` is passed as the argument. The function then iterates
over the nodes and sums the values until it encounters a NULL pointer. The final
sum is returned as `uint32_t`. The linked list node contains a value `(int32_t)`
as well as a pointer to the next node `(struct ll_node*)`;

