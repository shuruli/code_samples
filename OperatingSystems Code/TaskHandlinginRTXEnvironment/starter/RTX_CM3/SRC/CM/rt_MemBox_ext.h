/*----------------------------------------------------------------------------
 *      ECE254 Lab3
 *----------------------------------------------------------------------------
 *      Name:    RT_MEMBOX_EXT.H
 *      Purpose: Interface functions for blocking 
 *               fixed memory block management system
 *      Rev.:    V4.60
 *----------------------------------------------------------------------------
 *      This code extends the RealView Run-Time Library.
 *      Created by University of Waterloo ECE254 Lab Staff.
 *---------------------------------------------------------------------------*/
/* Variables */

/* Functions */
extern void     *rt_alloc_box_s (void *mpool);
extern struct OS_XCB os_blocked;
#define WAIT_MEM 10
extern OS_RESULT rt_free_box_s  (void *mpool, void *ptr);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

