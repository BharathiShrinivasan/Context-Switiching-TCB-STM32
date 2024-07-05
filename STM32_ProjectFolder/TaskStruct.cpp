#include<stdio.h>

void function1(void);
void function2(void);

struct TaskDetail
{
	int PSPAddress;
	void *PCAdress;
}MyTasks[2]={0x01,(void *)&function1,0x02,(void *)&function2};

void function1 (void)
{
	int x=0;
}

void function2(void)
{
	int y=0;
}

int main()
{
	unsigned int x=0;
	x=(int)MyTasks[0].PCAdress;
	printf("%d,%d",MyTasks[0].PSPAddress,MyTasks[0].PCAdress);
	return 1;
}
