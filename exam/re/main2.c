#include <stdio.h>


struct tree_node {
	struct tree_node *lhs;
	struct tree_node *rhs;
	char* key;
	char* value;
};

extern char* task2(struct tree_node*, char*);

int main(void)
{

	struct tree_node leafa = {NULL, NULL, "a", "valuea"};
	struct tree_node leafc = {NULL, NULL, "c", "valuec"};
	struct tree_node leafb = {&leafa, &leafc, "b", "valueb"};

	struct tree_node leafe = {NULL, NULL, "e", "valuee"};
	struct tree_node leafg = {NULL, NULL, "g", "valueg"};
	struct tree_node leaff = {&leafe, &leafg, "f", "valuef"};

	struct tree_node leafd = {&leafb, &leaff, "d", "valued"};

	char* b = task2(&leafd,"h");
	printf("It worked : %s\n", b);
	return 0;
}
