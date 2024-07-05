//To understand TCB's function and mimic one here. Context Switching as Round-robbin scheme tiggered by systick.
#include "stm32f4xx.h"

/* Discovery Board by-default sys clock and ABP running in 16MHz */
/*************************************************************************************************************************************************************************/
int main(void) //__main loop()
{
	// Thread mode , Privilege access
	// Initialise __main PSP, set un-privilege, thread mode, reset Floating point
	__asm("	LDR R1,=0X20000200\n"
				"	MSR PSP,R1\n" 
				"	MOV R0,#3\n"
				"	MSR CONTROL,R0");
				
	__asm("SVC 0XF3");	// OS initialisation to be done using SVC exception
	
	/* Things to be done in OS Initialisation -
		1. Allocate Process SP to n-tasks {PSP_0, PSP_1 ... PSP_n} min size to be of 8 word fram / 8*4byte=32Bytes
		2. Initialise stack frame as {xPSR,PC,LR,R12,R3,R2,R1,R0} as {0x0100 0000,=Task_nFunctionLable,0xFFFF FFFF,0,0,0,0,0}
		3. Decalre a memory to hold the current task no. being executed. {x ; x in {1,2,..n}} which can then be used to workout scheduling
		4. Initialise Systick, reload count, priority=4, enable
		5. Set PendSV priority=6
		6. Set TIMER2_IRQ enable, set priority=5.
		7. Exit
	*/
	
	while(1)
	{
		__asm("nop"); //Once User tasks start this section is not reached undefinitly
	}
}

void Systick_initialize (void) //Initialise Systick, reload count, priority=4, enable
{
	SysTick ->CTRL = 0; 
	SysTick->LOAD = 0x00F42400 - 1; // 1sec=(16*10^6)*0xF42400
	SysTick->VAL = 0; 
	SysTick->CTRL |= (1U<<0 | 1U<<1 | 1U<<2) ;//enable clock source, interrupt, enable systick
	NVIC_SetPriorityGrouping(3); // Set priotiy grouping
	SCB->SHP[11]=0x40; // priority 4 subgroup 0
}

/*******************************************************Users Task functions ************************************************************************************************/

volatile static uint32_t Task1_Live=0;
void Task1_main(void)
{
	
	while(1)
	{
		Task1_Live++;
		for(uint32_t i=0;i<4000;i++)
		__ASM("nop");
	}
}

volatile static uint32_t Task2_Live=0;
void Task2_main(void)
{
	
	while(1)
	{
		Task2_Live++;
		for(uint32_t i=0;i<4000;i++)
		__ASM("nop");
	}
}

volatile static uint32_t Task3_Live=0;
void Task3_main(void)
{

	while(1)
	{
		Task3_Live++;
		for(uint32_t i=0;i<4000;i++)
		__ASM("nop");
	}
}
volatile static uint32_t Task4_Live=0;
void Task4_main(void)
{

	while(1)
	{
		Task4_Live++;
		for(uint32_t i=0;i<4000;i++)
		__ASM("nop");
	}
}
volatile static uint32_t Task5_Live=0;
void Task5_main(void)
{

	while(1)
	{
		Task5_Live++;
		for(uint32_t i=0;i<4000;i++)
		__ASM("nop");
	}
}

/***************************************************************** Exception handlers***********************************************************************
	SVC 0XF3 -> OS initialisation
	PendSV -> Task switching / Scheduler algorithm
	SysTick -> set PendSV = pending @ every 5 sec, IRQ=pending @ every 15sec
	IRQ -> Sevice Timer IRQ
************************************************************************************************************************************************************/

volatile static uint8_t svc_number; //to hold SVC_number
volatile static uint32_t PSP_Task[5]={0X2000F300,0X2000F350,0X2000F400,0X2000F450,0X2000F500}; //PSP array initialised for individual tasks
volatile static uint32_t SPAddress=0x00,UpdateSP=0x00;

void SVC_Handler(void){ 	// OS initialisation SVC 0XF3
	__ASM("push {lr}"); //LR hold the exception return code need to push it in MSP
	__ASM("TST LR,#4"); //Test bit number 2-> 0 @return MSP, 1 @return PSP
	__ASM("ITE EQ\n"
				"MRSEQ R0,MSP\n"
				"MRSNE R0,PSP");
	__ASM("LDR R0,[R0,#24]"); //R0 copies the address of next to svc call
	__ASM("LDRB R0,[R0,#-2]"); //[R0-2]->R0 {BYTE copy} that gets the svc_number
	__ASM("LDR R7,=svc_number");
	__ASM("STR R0,[R7]");
	
	if(svc_number==0XF3) //SVC number is 0XF3 do the OS initialisation here
	{
	
		// 2. Initialise stack frame as {xPSP,PC,LR,R12,R3,R2,R1,R0} as {0x0100 0000,=Task_nFunctionLable,0xFFFF FFFF,0,0,0,0,0}
			
		//TASK -1
			__asm("	LDR R1,=0X2000F300\n");
			__asm("	LDR R0,=0x00000000\n");
			__asm("	STR R0,[R1],#4\n"); //R0
			__asm("	STR R0,[R1],#4\n"); //R1
			__asm("	STR R0,[R1],#4\n"); //R2
			__asm("	STR R0,[R1],#4\n"); //R3
			__asm("	STR R0,[R1],#4\n" );//R12
			__asm("	LDR R0,=0xFFFFFFFF\n");
			__asm("	STR R0,[R1],#4\n"); //LR
			__asm("	LDR R0,=Task1_main\n");
			__asm("	STR R0,[R1],#4\n"); //PC
			__asm("	LDR R0,=0x01000000\n");
			__asm("	STR R0,[R1],#4\n"); //xPSR		
			
		//TASK -2
			__asm("	LDR R1,=0X2000F350\n");
			__asm("	LDR R0,=0x00000000\n");
			__asm("	STR R0,[R1],#4\n"); //R0
			__asm("	STR R0,[R1],#4\n"); //R1
			__asm("	STR R0,[R1],#4\n"); //R2
			__asm("	STR R0,[R1],#4\n"); //R3
			__asm("	STR R0,[R1],#4\n" );//R12
			__asm("	LDR R0,=0xFFFFFFFF\n");
			__asm("	STR R0,[R1],#4\n"); //LR
			__asm("	LDR R0,=Task2_main\n");
			__asm("	STR R0,[R1],#4\n"); //PC
			__asm("	LDR R0,=0x01000000\n");
			__asm("	STR R0,[R1],#4\n"); //xPSR	
			
		//TASK -3
			__asm("	LDR R1,=0X2000F400\n");
			__asm("	LDR R0,=0x00000000\n");
			__asm("	STR R0,[R1],#4\n"); //R0
			__asm("	STR R0,[R1],#4\n"); //R1
			__asm("	STR R0,[R1],#4\n"); //R2
			__asm("	STR R0,[R1],#4\n"); //R3
			__asm("	STR R0,[R1],#4\n" );//R12
			__asm("	LDR R0,=0xFFFFFFFF\n");
			__asm("	STR R0,[R1],#4\n"); //LR
			__asm("	LDR R0,=Task3_main\n");
			__asm("	STR R0,[R1],#4\n"); //PC
			__asm("	LDR R0,=0x01000000\n");
			__asm("	STR R0,[R1],#4\n"); //xPSR
			
		//TASK -4
			__asm("	LDR R1,=0X2000F450\n");
			__asm("	LDR R0,=0x00000000\n");
			__asm("	STR R0,[R1],#4\n"); //R0
			__asm("	STR R0,[R1],#4\n"); //R1
			__asm("	STR R0,[R1],#4\n"); //R2
			__asm("	STR R0,[R1],#4\n"); //R3
			__asm("	STR R0,[R1],#4\n" );//R12
			__asm("	LDR R0,=0xFFFFFFFF\n");
			__asm("	STR R0,[R1],#4\n"); //LR
			__asm("	LDR R0,=Task4_main\n");
			__asm("	STR R0,[R1],#4\n"); //PC
			__asm("	LDR R0,=0x01000000\n");
			__asm("	STR R0,[R1],#4\n"); //xPSR

		//TASK -5
			__asm("	LDR R1,=0X2000F500\n");
			__asm("	LDR R0,=0x00000000\n");
			__asm("	STR R0,[R1],#4\n"); //R0
			__asm("	STR R0,[R1],#4\n"); //R1
			__asm("	STR R0,[R1],#4\n"); //R2
			__asm("	STR R0,[R1],#4\n"); //R3
			__asm("	STR R0,[R1],#4\n" );//R12
			__asm("	LDR R0,=0xFFFFFFFF\n");
			__asm("	STR R0,[R1],#4\n"); //LR
			__asm("	LDR R0,=Task5_main\n");
			__asm("	STR R0,[R1],#4\n"); //PC
			__asm("	LDR R0,=0x01000000\n");
			__asm("	STR R0,[R1],#4\n"); //xPSR

		//4. Initialise Systick, reload count, priority=4, enable
			Systick_initialize(); // sys tick handler priority 4 subgroup 0
			
		//5. Set PendSV priority=6
			SCB->SHP[10]=0x60; //we're setting PensSV ka priority which is 6, and subpriority 0
			
		//6. Set TIMER2_IRQ enable, set priority=5.
			NVIC_SetPriority(TIM2_IRQn,5); // IRQ28 or TIMER2_IRQ
			NVIC->ISER[0]|=(1U<<28);
		
		//7. Exit
		__ASM("pop {pc}"); // pop LR from MSP and copy to PC -> this will mark exception exit.
	}
	else
	{
		//7. Exit
		__ASM("pop {pc}"); // pop LR from MSP and copy to PC -> this will mark exception exit.
	}
}

volatile static uint32_t HeartBeat=0; // used to mark 5sec and 15sec
void SysTick_Handler(void)
{
	HeartBeat++;
	if(HeartBeat%5==0) // every 5 sec
	{	
		SCB->ICSR|=SCB_ICSR_PENDSVSET_Msk;	//SET PENDSV TO PENDING
	}
	if(HeartBeat%15==0) // 15secs
	{
		HeartBeat=0;
		NVIC->ISPR[0]|=(1U<<28); // SET TIMER INTERRUPT AS PENDING every 15Sec
		__asm("nop");
	}
}

volatile static uint32_t TaskToSelect=(uint32_t)-1; //Currently running task no. {0-TASK1, 1-TASK2..., -1=INITIALISATION}
void PendSV_Handler(void) //Scheduler round robin
{
	if(TaskToSelect!=(uint32_t)-1){ // to exclude while init process
	// update PSP of previous task since some stack usage would be carried at task level
	__asm("	LDR R1,=UpdateSP\n");
	__asm("	MRS R2,PSP\n");
	__asm("	STR R2,[R1]\n");
	PSP_Task[TaskToSelect]=UpdateSP; // Update the PSP array
	}
	
	TaskToSelect=(TaskToSelect+1)%5; //0,1,2,3,4,0,1.. round robin
	SPAddress=PSP_Task[TaskToSelect]; // next context ka PSP
	__asm("	LDR R1,=SPAddress\n"); // copy new PSP
	__asm("	LDR R1,[R1]\n");
	__asm("	MSR PSP,R1\n"); // 
	
}	

volatile static uint32_t InterruptCount=0x00;
void TIM2_IRQHandler (void)   // To service timer interrupt
{ 
	InterruptCount++;
	__asm("nop");
} 
