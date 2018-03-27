#include <stdio.h>
#define JUST_CHECKING
#define LIMIT 4

int main(viod)
{
	int i;
	int total=0;

	for(i=1;i<=4;i++)
	{
		total+=2*i*i+1;
#ifdef JUST_CHECKING
	 printf("i=%d,total=%d\n",i,total);
#endif
	}
     printf("total=%d\n",total);
	 return 0;
}