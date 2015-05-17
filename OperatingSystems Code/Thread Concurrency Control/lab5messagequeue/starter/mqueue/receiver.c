/**
 * @file:   code/lab5/mqueue/receiver.c
 * @brief:  receive a message from POSIX queue. Typle Ctrl-C to quit 
 * @date:   2014/06/25
 * NOTES:
 * To execute: <executable> <arg1>, where arg1 is a posix queue name.
 * EXAMPLE: ./receiver.out /mailbox1_userid 
 * A valid posix queue name must start with a "/". 
 * Execute command "man mq_overview" for details.
 */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <signal.h>
#include "common.h"
#include "point.h"
#include <math.h>
#include <semaphore.h>

#define _XOPEN_SOURCE 600

bool g_continue = true;

void sig_handler(int sig)
{
	g_continue = false;
}

int main(int argc, char *argv[])
{
	mqd_t qdes;
	char  *qname = NULL;
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;

	/* Open a preexisting semaphore. */
	sem_t *sem; 
	
	/* ensuring that the two arguments required are coming in*/
	if ( argc !=2 ) {
		printf("You must specify your id value"); 
		printf("You must specify your B value"); 
		exit(1);
	}
	int queue_size = atoi(argv[0]);	/*message queue size*/
	int id = atoi(argv[1]);	/*id of consumer*/
	
	sem = sem_open("/num_consumed", 0); /* Open a preexisting semaphore. */
	if(sem==SEM_FAILED){
		printf("Error opening sem\n");
	}
	
	/* setting a default name for our mailbox*/
	qname = "/mailbox1_sshuruli";
	attr.mq_maxmsg  = queue_size;
	attr.mq_msgsize = sizeof(struct point);
	attr.mq_flags   = 0;	/* a blocking queue  */

	/*opening message queue*/
	qdes  = mq_open(qname, O_RDONLY, mode, &attr);
	if (qdes == -1 ) {
		printf("Error in opening the mailbox in the consumer.");
		perror("mq_open()");
		exit(1);
	}

	signal(SIGINT, sig_handler);	/* install Ctl-C signal handler */

	/* we will only consume up until all the items have been consumed*/
	while(sem_trywait(sem)!=-1){

		struct point pt;
		struct timespec ts = {time(0) + 5, 0};

		/* only block for a limited time if the queue is empty */
		if (mq_timedreceive(qdes, (char *) &pt, sizeof(struct point), 0, &ts) == -1) {
			perror("mq_timedreceive() failed");
			printf("Type Ctrl-C and wait for 5 seconds to terminate.\n");
		} 
		else {
			
			/*consuming integer and finding square root*/
			int x = get_x_coord(pt);
			double sqrt_num = sqrt(x);

			/*if the square root number is an integer, print the result*/
			if(sqrt_num-floor(sqrt_num) < 1e-8) {
				printf("%d %d %d \n",id, x, (int) sqrt_num);
			}

		}

	}

	/*close the semaphore*/
	sem_close(sem);

	/* we must close the connection to the queue still*/
	if (mq_close(qdes) == -1) {
		perror("mq_close() failed");
		exit(2);
	}

	return 0;
}
