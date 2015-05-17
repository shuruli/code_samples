/**
 * @file:   code/lab5/mqueue/top.c
 * @brief:  send random points to a POSIX queue. Type 'q' to terminate. 
 * @date:   2014/06/25
 * NOTES:
 * To execute: <executable> <arg1>, where arg1 is a posix queue name.
 * EXAMPLE: ./sender.out /mailbox1_userid 
 * A valid posix queue name must start with a "/". 
 * Execute command "man mq_overview" for details.
 * Check /dev/mqueue to clean up the queue if sender fails to do so.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include "common.h"
#include "point.h"
#include <wait.h>
#include <semaphore.h>

int spawn (char* program, char** arg_list)
{
  pid_t child_pid;

  /* Duplicate this process.  */
  child_pid = fork ();
  if (child_pid != 0)
    /* This is the parent process.  */
    return child_pid;
  else {
    /* Now execute PROGRAM, searching for it in the path.  */
    execvp (program, arg_list);
    /* The execvp function returns only if an error occurs.  */
    fprintf (stderr, "an error occurred in execvp\n");
    abort ();
  }
}

int main(int argc, char *argv[])
{
	int child_status;/* this variable is only used for wait*/
	
	mqd_t qdes; /*message queue for data*/
	sem_t *sem; /*semaphore that counts number consumed*/
	int i,j;
	
	/*time values for timing analysis*/
	struct timeval tv;
	double t1;
	double t2;
	
	/*msesage queue attributes*/
	char *qname = NULL;
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;

	/* ensuring that the five arguments required are coming in*/
	if ( argc !=5 ) {
		printf("Usage: %s <qname>\n", argv[0]);
		printf("The qname must start with a \"/\". \n");
		printf("You must specify your N value"); 
		printf("You must specify your B value"); 
		exit(1);
	}

	/* capacity = N*/
	int capacity = atoi(argv[1]);
	int B = atoi(argv[2]); /*size of queue*/
	int P = atoi(argv[3]);	/*number of producers*/
	int C = atoi(argv[4]);	/*number of consumers*/
	
	if( B <= 0 ) {
		printf("The value B must be greater than 0"); 
		exit(1);		
	}
	
	if( P <= 0 ) {
		printf("The value P must be greater than 0"); 
		exit(1);
	}
	
	if( C <= 0 ) {
		printf("The value C must be greater than 0"); 
		exit(1);
	}
	
	qname = "/mailbox1_sshuruli";/*default mailbox name*/

	/*max queue size*/
	attr.mq_maxmsg  = B;
	attr.mq_msgsize = sizeof(struct point);
	attr.mq_flags   = 0;

	qdes  = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
	if (qdes == -1 ) {
		printf("Opening the queue failed in the top.c");
		perror("mq_open() failed");
		exit(1);
	}
	
	/*initializing semaphore with total number of numbers created*/
	sem = sem_open("/num_consumed", O_CREAT | O_EXCL, 0644, capacity); 
	if (sem == SEM_FAILED)
	{
		printf ("Could not open semaphore --- errno \n");
	}
	
	/* building our receivers process's argument list*/
	char* arg_list_reciever[] = {
		argv[2],
		"0",
		NULL

	};
	
	/*building our producers process's argument list*/
	char* arg_list_producer[] = {
		argv[1],
		argv[3],
		"0",
		argv[2],
		NULL
	};

	/*time before first child is spawned*/
	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + tv.tv_usec/1000000.0;

	/*spawning C consumers*/
	for(j=0;j<C;j++) {

		char str[15];
		sprintf(str, "%d", j);
		arg_list_reciever[1] = str;

		//receiver process
		spawn("./receiver.out", arg_list_reciever);
	}
	
	/*spawning P producers*/
	for(i=0;i<P;i++) {
		char str[15];
		sprintf(str, "%d", i);
		arg_list_producer[2] = str;
		
		spawn("./sender.out", arg_list_producer);
	}
	
	/* before unlinking queue we must wait for child to finish*/
	for (i = 0; i < (C+ P); i ++){
		wait(&child_status); 
	}

	if (WIFEXITED (child_status)){
		
		/*unlinking and closing message queue*/
		if (mq_close(qdes) == -1){
			perror("mq_close() failed"); 
			exit(2); 
		}
		if (mq_unlink(qname) != 0){
			perror("mq_unlink() failed");
			exit(3); 
		} 
		
		/*closing and unlinking semaphore*/
		sem_close(sem); 
		sem_unlink("num_consumed");

		/*time after all numbers are consumed*/
		gettimeofday(&tv, NULL);
		t2 = tv.tv_sec + tv.tv_usec/1000000.0;
		
	}
	else{
		printf ("the child process exited abnormally\n");
	}
	
	printf("System execution time: %.6lf seconds\n",t2-t1);

	return 0;
}
