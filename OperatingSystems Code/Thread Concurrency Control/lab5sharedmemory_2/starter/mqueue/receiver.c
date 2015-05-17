/**
 * @file:   code/lab4/mqueue/receiver.c
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
	struct timeval tv1;
	double t3;
	double t4;
	/* checking to make sure only two arguments come in */
	if ( argc !=2 ) {
		printf("Usage: %s <qname>\n", argv[0]);
		printf("The qname must start with a \"/\". \n");
		printf("An example qname: /mailbox1_userid\n");
		exit(1);
	}
	/* setting a default name for our mailbox*/
	qname = "/mailbox1_userid";

	attr.mq_maxmsg  = QUEUE_SIZE;
	attr.mq_msgsize = sizeof(struct point);
	attr.mq_flags   = 0;	/* a blocking queue  */
	int counter = 0; 
	qdes  = mq_open(qname, O_RDONLY, mode, &attr);
	if (qdes == -1 ) {
		perror("mq_open()");
		exit(1);
	}

	signal(SIGINT, sig_handler);	/* install Ctl-C signal handler */

	srand(time(0));
	int j; 
	/* we will only consume up until the number of items in the queue*/
	for (j = 0; j < atoi(argv[0]); j++){
		struct point pt;
		struct timespec ts = {time(0) + 5, 0};

		/* only block for a limited time if the queue is empty */
		if (mq_timedreceive(qdes, (char *) &pt, \
		    sizeof(struct point), 0, &ts) == -1) {
			perror("mq_timedreceive() failed");
			printf("Type Ctrl-C and wait for 5 seconds to terminate.\n");
		} else {
			printf("random num %d\n", \
		                get_x_coord(pt));

		}

	}
	/* we must close the connection to the queue still*/
	if (mq_close(qdes) == -1) {
		perror("mq_close() failed");
		exit(2);
	}

	return 0;
}
