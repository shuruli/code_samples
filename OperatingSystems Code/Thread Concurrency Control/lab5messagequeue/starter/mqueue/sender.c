/**
 * @file:   code/lab4/mqueue/sender.c
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

int main(int argc, char *argv[])
{

	mqd_t qdes;

	int i; 

	char *qname = NULL;
	mode_t mode = S_IRUSR | S_IWUSR;
	struct mq_attr attr;
	struct point pt;

		/* ensuring that the four arguments required are coming in*/
	if ( argc !=4 ) {
		printf("You must specify your N value\n");
		printf("You must specify your P value"); 
		printf("You must specify your id value"); 
		printf("You must specify your B value"); 
		exit(1);
	}
	
	/* capacity = N*/
	int capacity = atoi(argv[0]);
	int P=atoi(argv[1]); /*number of producers*/
	int id=atoi(argv[2]);	/*id of producer*/
	int queue_size = atoi(argv[3]);	/*queue size*/

	/*default mailbox name*/
	qname = "/mailbox1_sshuruli";
	attr.mq_maxmsg  = queue_size;
	attr.mq_msgsize = sizeof(struct point);
	attr.mq_flags   = 0;		/* a blocking queue  */

	/*opening message queue*/
	qdes  = mq_open(qname, O_RDWR, mode, &attr);
	if (qdes == -1 ) {
		printf("Error in opening the mailbox in the producer.");
		perror("mq_open()");
		exit(1);
	}
		
	for(i=id; i< capacity; i+=P) {
		set_position(i, &pt);

		/* creating number and sending it in the message queue*/ 
		if (mq_send(qdes, (char *)&pt, sizeof(struct point), 0) == -1) {
			fprintf(stderr, "%s\n","Error in sending"); 
			perror("mq_send() failed");
		}

	}
	/*closing message queue*/
	if (mq_close(qdes) == -1){
		perror("mq_close() failed"); 
		exit(2); 
	}

	return 0;
}
