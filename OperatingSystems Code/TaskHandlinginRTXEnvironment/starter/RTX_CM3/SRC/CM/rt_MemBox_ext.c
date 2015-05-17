/*----------------------------------------------------------------------------
 *      ECE254 Lab3
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMBOX_ext.C
 *      Purpose: Interface functions for blocking 
 *               fixed memory block management system
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *      This code is extends the RealView Run-Time Library.
 *      Created by University of Waterloo ECE254 Lab Staff.
 *---------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------------
 *      Includes
 *---------------------------------------------------------------------------*/
 
#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_System.h"`
#include "rt_MemBox.h"
#include "rt_HAL_CM.h"
#include "rt_Task.h"
#include "rt_task_ext.h" /* added in ECE254 lab3 keil_proc */ 
#include "rt_MemBox_ext.h" /* added in ECE254 lab3 keil_proc */   

/* ECE254 Lab3 Comment: You may need to include more header files */

/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/
 // This is the queue which will contain all of our blocked tasks in order of priority 
struct OS_XCB os_blocked;

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

/*==========================================================================*/
/*  The following are added for ECE254 Lab3 Proc Management Assignmet       */
/*==========================================================================*/

/*---------------- rt_alloc_box_s, task blocks when out of memory-----------*/

/**  
   @brief: Blocking memory allocation routine.
 */
void *rt_alloc_box_s (void *p_mpool) {
	 
	/* Add your own code here. Change the following line accordingly */
	

		// pointer that holds the return value of the allocation of this memory pool 	
		U8 *box;
		
		box  = rt_alloc_box (p_mpool);
		
		// if box is NULL then we do not have enough memory 
		if(box == NULL) {				
				
				// if the os_blocked's p_lnk is null then the put_prio does not expect 
				// this and therefore we have to check for that 
				if(((P_TCB) &os_blocked)->p_lnk!=NULL) {
					
					// if it is not null then we put it into our blocked queue 
					rt_put_prio((P_XCB)&os_blocked,os_tsk.run);

				}else{
					// the p_lnk is null then we must set our os_blocked's p_lnk	
					((P_TCB)&os_blocked)->p_lnk = os_tsk.run;
					os_tsk.run->p_lnk = NULL;
					os_tsk.run->p_rlnk =(P_TCB)&os_blocked;
					
				}
				
				// block with a large delay and our own declared state of WAIT_MEM 
				rt_block(0xffff,WAIT_MEM);
				
		}else{
			
			// if the memory is not full then just return the address returned by alloc_box 
			return box;
		}
}


/*----------- rt_free_box_s, pair with _s memory allocators ----------------*/
/**
 * @brief: free memory pointed by ptr, it will unblock a task that is waiting
 *         for memory.
 * @return: OS_R_OK on success and OS_R_NOK if ptr does not belong to gp_mpool 
 */
OS_RESULT rt_free_box_s (void *p_mpool, void *box) {
	/* Add your own code here. Change the following line accordingly */
	
	// this variable will hold if rt_free_box contains an error 
	int free_box_error;
	
	// checking to see if the memory block we are trying to free exists within our memory pool declared 
	if( box < p_mpool || ((P_BM) p_mpool)->end < box) {
			return (OS_R_NOK);
	}
	
	//free the box and store the return value in a variable for check 
	free_box_error = rt_free_box(p_mpool, box); 
	
	// if there is no error then we proceed with 
	if(!free_box_error){
			
		P_TCB next;
		
		// let our variable P_TCB contain the task returned by popping our queue
		next = (P_TCB)rt_get_first((P_XCB)&os_blocked);
		
		// if next is not null then we set the ret_val value to the memory address at which 
		// we had initialized a block in our memory pool 
		if(next != NULL) {
			next->ret_val=(U32)box;
		}
		
		// we dispatch the next task. Even if next is null this will work just fine 
		rt_dispatch(next);
			
		// return OK because we know we do not have an error 
		return (OS_R_OK);
		
	}
	
	
	// if we have an error then we return os_r_nok 
	return (OS_R_NOK);
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
