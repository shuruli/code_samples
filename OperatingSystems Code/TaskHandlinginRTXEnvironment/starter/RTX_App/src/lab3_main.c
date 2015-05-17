/**
 * @brief: ECE254 lab3 starter file that calls os_tsk_get()
 * @file: lab3_main.c
 * @date: 2014/05/28
 */
/* NOTE: This release is for students to use */

#include <LPC17xx.h>
#include "uart_polling.h" 
#include <RTL.h>
#include "../../RTX_CM3/INC/RTL_ext.h" /* extended interface header file */
#include <stdio.h>
#include <string.h>

#define NUM_FNAMES 10
_declare_box(mympool,8,1);

struct func_info {
  void (*p)();      /* function pointer */
  char name[16];    /* name of the function */
};

extern void os_idle_demon(void);
__task void task1(void);
__task void task2(void);
__task void task3(void);
__task void task4(void);
__task void task5(void);
__task void task6(void);
__task void task7(void);
__task void task10(void);
__task void init (void);
 
char *state2str(unsigned char state, char *str);
char *fp2name(void (*p)(), char *str);

OS_MUT g_mut_uart;
OS_TID g_tid = 255;

int  g_counter = 0;  // a global counter
int p_counter = 0;
char g_str[16];
char g_tsk_name[16];
U32 num_tasks = 9; 
void *temp1 = NULL;
void *temp2 = NULL;
void *temp3 = NULL;
void *temp4 = NULL; 

struct func_info g_task_map[NUM_FNAMES] = \
{
  /* os_idle_demon function ptr to be initialized in main */
  {NULL,  "os_idle_demon"}, \
  {task1, "task1"},   \
  {task2, "task2"},   \
  {task3, "task3"},   \
  {task4, "task4"},   \
	{task5, "task5"},		\
  {task6, "task6"},   \
  {task7, "task7"},   \
	{task10, "task10"},		\
  {init,  "init" }
};

__task void task1(void)
{
	
	// because our memory pool has one block, one allocation should fill up the pool 
	temp1 = os_mem_alloc(mympool);
	printf("task1 allocating memory\n");
	
	// do a wait so that we switch tasks. This wait will cause Task10 to run first 
	// which will display the status of all tasks. When task10 issues IT'S wait
	// we will proceed to task2 
	os_dly_wait(10);
	
	printf("Deleting task1\n");
	os_tsk_delete_self();	
}

__task void task2(void)
{
	
	// this will try to allocate and get blocked since task 1 filled up our pool 
	temp2 = os_mem_alloc(mympool);
	printf("task2 is unblocked\n");
	
	// this delay is for us to be able to show the status of the status after it is unblocked 
	os_dly_wait(10);
	printf("deleting task2\n");
	
	// we delete the tasks because at this point we are done with testing them 
	// and the output from the tasks are sufficient to show functionality of our program 
	os_tsk_delete_self();
		
}

__task void task3(void)
{
	printf("task3 allocating memory\n");
	// this will try to allocate and get blocked since task 1 filled up our pool
// and task2 got blocked 	
		temp3 = os_mem_alloc(mympool);
	printf("task3 is unblocked\n");
	
	//same idea as task2
		os_dly_wait(10);
	printf("deleting task3\n");

		os_tsk_delete_self();

}

__task void task4(void)
{
	printf("task4 allocating memory\n");
	// this will try to allocate and get blocked since task 1 filled up our pool 
	// and task 3 got blocked
	temp4 = os_mem_alloc(mympool);
	printf("task4 is unblocked\n");
	
	//same idea as task2 
	os_dly_wait(10);
	printf("deleting task4\n");

	os_tsk_delete_self();

}
__task void task5(void)
{
	printf("freeing memory task 5\n");
	// this task will free the memory pool that was allocated by task1 
	// thereby allowing task2 to fill up. Then the task2 will issue a dly_wait which
	// will allow task6 to run 
	os_mem_free(mympool, temp1);	
	os_dly_wait(10);
	printf("deleting task5\n");

	os_tsk_delete_self();
}

__task void task6(void)
{
	printf("freeing memory - task6\n");
	// since task2's allocation was successful and it issued a dly_wait() we are 
	// going to free up the memory pool task2 used up to allow task3 to run 
	// when task3 issues the wait we will enter task7 since it is the next ready task
	// with highest prio 
	os_mem_free(mympool, temp2);

	os_dly_wait(10);
	printf("deleting task6\n");
	os_tsk_delete_self();

}

// task7 will test the stack usage as well as freeing up task3's memory block 
// to allow task4 to run 
__task void task7(void)
{
	char test[256];
	
	int i;
	char a;
	
	printf("freeing memory - task7\n");
	// when we free up temp3 we put task4 back into ready 
	os_mem_free(mympool, temp3); 
	// when we issue a wait we will switch to task4 and allocate memory 
	os_dly_wait(10);
	printf("Increasing stack usage by 50%\n");

	// we are showing the filling up of our stack. This is testing part A 
	//we created an array of size 256 which is half of our stack size
	for(i=0;i<256;i++) {
		test[i] = 'a';
	}
	for(;;){
		//need to use the array so that the compiler doesn't optimize it out
		a= test[0];
	}

}


/*--------------------------- task10 -----------------------------------*/
/* checking states of all tasks in the system                          */
/*---------------------------------------------------------------------*/
__task void task10(void)
{
	
	for(;;){
		U8 i=1;
		RL_TASK_INFO task_info;
		
		os_mut_wait(g_mut_uart, 0xFFFF);
		printf("TID\tNAME\t\tPRIO\tSTATE   \t%%STACK\n");
		os_mut_release(g_mut_uart);
			
		for(i = 0; i <9; i++) { // this is a lazy way of doing loop.
			if (os_tsk_get(i+1, &task_info) == OS_R_OK) {
				os_mut_wait(g_mut_uart, 0xFFFF);  
				printf("%d\t%s\t\t%d\t%s\t%d%%\n", \
							 task_info.task_id, \
							 fp2name(task_info.ptask, g_tsk_name), \
							 task_info.prio, \
							 state2str(task_info.state, g_str),  \
							 task_info.stack_usage);
				os_mut_release(g_mut_uart);
			} 
		}
			
		if (os_tsk_get(0xFF, &task_info) == OS_R_OK) {
			os_mut_wait(g_mut_uart, 0xFFFF);  
			printf("%d\t%s\t\t%d\t%s\t%d%%\n", \
						 task_info.task_id, \
						 fp2name(task_info.ptask, g_tsk_name), \
						 task_info.prio, \
						 state2str(task_info.state, g_str),  \
						 task_info.stack_usage);
			os_mut_release(g_mut_uart);
		} 

		// this task will run at the start of every other task switch if it is ready 
		// since it is the highest priority. This short dly_wait() is to switch to another task 
		os_dly_wait(1);
	}
}

/*--------------------------- init ------------------------------------*/
/* initialize system resources and create other tasks                  */
/*---------------------------------------------------------------------*/
__task void init(void)
{
	// initialize our mutex 
	os_mut_init(&g_mut_uart);
	
	// we will wait on the mutex and initialize all of our functions then release the mutex. 
	// this ensures that tasks do not begin until all are initialized
	
	os_mut_wait(g_mut_uart, 0xFFFF);
	// initialize our mem pool 
	_init_box (mympool, sizeof (mympool), 8);

	os_mut_release(g_mut_uart); 
	
	os_mut_wait(g_mut_uart, 0xFFFF);
	printf("init: TID = %d\n", os_tsk_self());
	
	// the task that displays the status of all the tasks is at the highest priority 
	g_tid = os_tsk_create(task10, 10);  /* task 10 at priority 10 */
	printf("init: created task10 with TID %d\n", g_tid);
	
	// the task that fills up the memory is going to be the next highest priority
	g_tid = os_tsk_create(task1, 9);  /* task 1 at priority 9 */
	printf("init: created task1 with TID %d\n", g_tid);
	
	// tasks 2 and 3 both of which try to allocate memory after task1
	// are at the same priority to illustrate the scenario where two tasks with the same priority
	// which are both blocked will get popped off the queue in order of the oldest to enter the queue
	g_tid = os_tsk_create(task2, 8);  /* task 2 at priority 8 */
	printf("init: created task2 with TID %d\n", g_tid);

	g_tid = os_tsk_create(task3, 8);  /* task 3 at priority 8 */
	printf("init: created task3 with TID %d\n", g_tid);
	
	// this task4 is at a lower priority than the two above but does the same thing 
	g_tid = os_tsk_create(task4, 7);  /* task 4 at priority 7 */
	printf("init: created task4 with TID %d\n", g_tid);
	
	// tasks 5, 6 and 7 free the memory block to unblock tasks 2 , 3 , 4
	g_tid = os_tsk_create(task5, 6);  /* task 5 at priority 6 */
	printf("init: created task5 with TID %d\n", g_tid);
	
		
	g_tid = os_tsk_create(task6, 5);  /* task 6 at priority 5 */
	printf("init: created task6 with TID %d\n", g_tid);
		
	g_tid = os_tsk_create(task7, 3);  /* task 7 at priority 3*/
	printf("init: created task7 with TID %d\n", g_tid);
	os_mut_release(g_mut_uart);
	
	os_tsk_delete_self();     /* task MUST delete itself before exiting */
}

/**
 * @brief: convert state numerical value to string represenation      
 * @param: state numerical value (macro) of the task state
 * @param: str   buffer to save the string representation, 
 *               buffer to be allocated by the caller
 * @return:the string starting address
 */
char *state2str(unsigned char state, char *str)
{
	switch (state) {
	case INACTIVE:
		strcpy(str, "INACTIVE");
		break;
	case READY:
		strcpy(str, "READY   ");
		break;
	case RUNNING:
		strcpy(str, "RUNNING ");
		break;
	case WAIT_DLY:
		strcpy(str, "WAIT_DLY");
		break;
	case WAIT_ITV:
		strcpy(str, "WAIT_ITV");
		break;
	case WAIT_OR:
		strcpy(str, "WAIT_OR");
		break;
	case WAIT_AND:
		strcpy(str, "WAIT_AND");
		break;
	case WAIT_SEM:
		strcpy(str, "WAIT_SEM");
		break;
	case WAIT_MBX:
		strcpy(str, "WAIT_MBX");
		break;
	case WAIT_MUT:
		strcpy(str, "WAIT_MUT");
		break;
	case WAIT_MEM:
		strcpy(str, "WAIT_MEM");
		break;		
	default:
		strcpy(str, "UNKNOWN");    
	}
	return str;
}

/** 
 * @brief: get function name by function pointer
 * @param: p the entry point of a function (i.e. function pointer)
 * @param: str the buffer to return the function name
 * @return: the function name string starting address
 */
char *fp2name(void (*p)(), char *str)
{
	int i;
	unsigned char is_found = 0;
  
	for ( i = 0; i < NUM_FNAMES; i++) {
		if (g_task_map[i].p == p) {
			str = strcpy(str, g_task_map[i].name);  
			is_found = 1;
			break;
		}
	}
	if (is_found == 0) {
		strcpy(str, "ghost");
	}
	return str;
}

int main(void)
 {
	SystemInit();         /* initialize the LPC17xx MCU */
	uart0_init();         /* initilize the first UART */
  
  
	/* fill the fname map with os_idle_demon entry point */
	g_task_map[0].p = os_idle_demon;
  
	printf("Calling os_sys_init()...\n");
	os_sys_init(init);    /* initilize the OS and start the first task */
}
