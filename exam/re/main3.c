#include <stdio.h>



extern long task3(long, unsigned long);


int main(void)
{
	long param1 = 5;
	unsigned long param2 = 20;
	long b = task(param1,param2);
	long c = task3(param1,param2);
	printf("It worked : %ld,%ld\n",b,c);
	return 0;
}
