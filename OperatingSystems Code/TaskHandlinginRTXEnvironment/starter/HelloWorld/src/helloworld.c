/**
* @file: helloworld.c
* @brief: Two simple tasks running pseduo-parallelly
*/
#include <LPC17xx.h>
#include <RTL.h>
#include <stdio.h>
#include "uart_polling.h"
#include "../../RTX_CM3/INC/RTL_ext.h"
__task void task1()
{
unsigned int i = 0;
for(;; i++)
{
printf("Task1: %d\n", i);
os_dly_wait(100);
}
}
__task void task2()
{
while(1)
{
printf("Task2: HelloWorld!\n");
os_dly_wait(300);
}
}

__task void task3()
{
while(1)
{
printf("Task3!\n");
os_dly_wait(300);
}
}

__task void task4()
{
while(1)
{
printf("%s %i\n","Number of active tasks: ",rt_tsk_count_get());
os_dly_wait(300);
}
}

__task void task5()
{
while(1)
{
printf("Task5!\n");
os_dly_wait(300);
}
}


__task void init(void)
{
os_tsk_create(task1, 1); // task1 at priority 1
os_tsk_create(task2, 1); // task2 at priority 1
os_tsk_create(task3, 1); // task3 at priority 1
os_tsk_create(task4, 1); // task4 at priority 1
os_tsk_create(task5, 1); // task5 at priority 1
os_tsk_delete_self(); // must delete itself before exiting
}
int main ()
{
SystemInit();
uart0_init();
os_sys_init(init);
}
