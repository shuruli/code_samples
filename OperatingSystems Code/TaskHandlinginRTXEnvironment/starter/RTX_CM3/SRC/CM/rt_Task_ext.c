/*----------------------------------------------------------------------------
 *      ECE254 Lab3
 *----------------------------------------------------------------------------
 *      Name:    RT_TASK_ext.C
 *      Purpose: Interface functions for extended task management  
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *      This code extends the RealView Run-Time Library.
 *      Created by University of Waterloo ECE254 Lab Staff.
 *---------------------------------------------------------------------------*/

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"
#include "rt_Task.h"
#include "rt_List.h"
#include "rt_MemBox.h"
#include "rt_Robin.h"
#include "rt_HAL_CM.h"
#include "rt_Task_ext.h"

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 *      Local Functions
 *---------------------------------------------------------------------------*/
/*--------------------------- rt_tsk_count_get ------------------------------*/
/* added in ECE254 lab2 keil_rtx */
int rt_tsk_count_get (void) {
/* add your own code here */
/* change the following line to return the number of active tasks */
	//TODO:: COMPLETE THE LAB WORK HERE!!
	
	
	int num_active_tasks = 0; 
	int tid;
	
	// 
	for (tid = 1; tid <= os_maxtaskrun; tid++) {
    if (os_active_TCB[tid-1] != NULL) {
      num_active_tasks++; 
    }
  }
	
	if(os_idle_TCB.state != INACTIVE){
		num_active_tasks++; 
	}
	
	return (num_active_tasks);
}

/*--------------------------- rt_tsk_get ------------------------------------*/
/* added in ECE254 lab3 keil_proc */
OS_RESULT rt_tsk_get (OS_TID task_id, RL_TASK_INFO *p_task_info) {
	/* Add your own code here. Change the following lines accordingly */
	
	int tid;
	
	int found = 0;
	U16 size;
	float start;
	U32 current;
	
	// must go through all the tasks in the system 
	for (tid = 0; tid <= os_maxtaskrun; tid++) {
		
		// get the corresponding task from the array 
		P_TCB task = os_active_TCB[tid-1];
		
		// if the task is not null then we proceed to display the info about it
		if( task!= NULL) {
			
			// if the task we get is the task we want info for 
			if( task->task_id == task_id) {
				//this is the task that we are looking for	
				
				// set the necessary fields 
				p_task_info->task_id     = task_id;
				p_task_info->state       = task->state;
				p_task_info->prio        = task->prio;
				
				// if we are the running task then we use rt_get_psp() to get the current stack pointer 
				if(task->state == RUNNING) {
					
					current = rt_get_PSP();
	
				}
				// if not then we use this
				else{
					
					current = task->tsk_stack;
					
				}
				
				// size of the stack 
				size = (U16)os_stackinfo;
				
				// getting the start of the stack 
				start = (U32)(task->stack)- current+size;
	
				// calculating usage 	
				p_task_info->stack_usage = 100*(start)/size;
				
				//set the ptask 
				p_task_info->ptask = task->ptask;
					
				found = 1;
			}
			
		}
		
	}

	//return the appropriate value if we can get the task
	if(found) {
		return OS_R_OK;
	}
	
	return OS_R_NOK;
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
