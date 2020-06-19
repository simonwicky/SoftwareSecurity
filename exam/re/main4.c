#include <stdio.h>
#include <stdint.h>



extern long task4(long*, long*, unsigned long);


int main(void)
{
	long param1 = 0xdeadbeef;
	long param2 = 0;
	long param3 = 16;
	task4(&param1,&param2, param3);
	printf("It worked : %lx, %lx\n",param1, param2);
	return 0;
}
