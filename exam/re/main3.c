#include <stdio.h>



extern long task3(long, unsigned long);

long task(long base, unsigned long exponent){
	long table[4] = {1, base, base*base, base*base*base};
	long acc = 1;

	for (int i = 62; i >= 0; i-=2)
	{
		int index = 3 & (exponent >> i);
		acc = acc * acc * acc * acc;
		acc *= table[index];
	}
	return acc;
}

int main(void)
{
	long param1 = 5;
	unsigned long param2 = 20;
	long b = task(param1,param2);
	long c = task3(param1,param2);
	printf("It worked : %ld,%ld\n",b,c);
	return 0;
}
