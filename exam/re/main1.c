#include <stdio.h>
//#include <task1.c>


struct cell {
	struct cell *lhs;
	struct cell *rhs;
	int value;
};

extern long task1(struct cell*);

int main(int argc, char const *argv[])
{
	struct cell leafa = {NULL, NULL,3};
	struct cell leafb = {NULL, NULL,5};
	struct cell a = {&leafa, &leafb,4};
	long b = task1(&a);
	printf("It worked : %ld\n", b);
	return 0;
}
