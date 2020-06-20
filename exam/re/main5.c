#include <stdio.h>



extern long task5(unsigned long,unsigned long, long*, unsigned long*);



int main(void)
{
	unsigned long param_1 = 15;
	unsigned long param_2 = 10;
	long param_3 = 0;
	unsigned long param_4 = 0;
	long c = task5(param_1,param_2, &param_3,&param_4);
	printf("It worked : %ld,%ld,%ld\n",c, param_3, param_4);
	return 0;
}
