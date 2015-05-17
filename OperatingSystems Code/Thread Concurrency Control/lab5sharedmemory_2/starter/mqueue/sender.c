/**
 * @file:   code/lab5/mqueue/sender.c
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
#include <math.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>
#include "common.h"
#include "point.h"
#include <wait.h>

#include <semaphore.h> 

#define _XOPEN_SOURCE 600

int capacity = 0;	
int P = 0; 
int C = 0; 
int buffer_size = 0; 
struct mq_attr attr;

// These two structs are used to send parameters to the producer and consumer methods 
struct producer_parms
{
	/* The ID of the producer */
	int producer_id; 
};

struct consumer_params
{
	/* The ID of the consumer */
	int consumer_id; 
};

struct buffer_queue {
   int front,rear;
   int capacity;
   int *array;
}; 


/* -- This queue was found off the internet -- */
/* Reference: http://stackoverflow.com/questions/20619234/circular-queue-implementation */ 
struct buffer_queue* q(int size)
{
    struct buffer_queue *q=malloc(sizeof(struct buffer_queue));
    if(!q)
        return NULL;
    q->capacity=size;
    q->front=-1;
    q->rear=-1;
    q->array=malloc(q->capacity*sizeof(int));
    if(!q->array)
       return NULL;
    return q;
};

int isemptyqueue(struct buffer_queue *q) {
   return(q->front==-1);
}

int isfullqueue(struct buffer_queue *q) {
   return((q->rear+1)%q->capacity==q->rear);
}

void enqueue(struct buffer_queue *q,int x) {

   if(isfullqueue(q))
      printf("queue overflow\n");
   else{
      q->rear=(q->rear+1)%q->capacity;
      q->array[q->rear]=x;
      if(q->front==-1) {
         q->front=q->rear;
      }
   }
}

int dequeue(struct buffer_queue *q) {
   int data=0;

   if(isemptyqueue(q)) {
      printf("queue underflow");
      return 0;
   }
   else {
      data=q->array[q->front];
      if(q->front==q->rear)
         q->front=q->rear=-1;
      else
         q->front=(q->front+1)%q->capacity;
   }

   return data;
}
/* -- This queue was found off the internet -- */

struct buffer_queue *shared_queue = NULL;

// declaring semaphores and mutex 
sem_t consumers;
sem_t producers;
sem_t buffers;
pthread_mutex_t job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

// Consumer
int receiver(void* parameters)
{
	// local consumer params struct to contain our ID 
	struct consumer_params* c = (struct consumer_params*) parameters; 
	//run consumers until we are done consuming 
	while(1){		

		// do a sem wait here 
		// total = consumers, count = producers, buffer = buffers 
		// if we are done consuming then we break out of this consumer 
		if (sem_trywait(&consumers) == -1){
			break; 
		}

		// must wait on producers to produce something before we start consuming
		sem_wait(&producers);

		// mutex since our queue is a critical resource 
		pthread_mutex_lock(&job_queue_mutex); 
		int initial_buffer_number = dequeue(shared_queue);
		pthread_mutex_unlock(&job_queue_mutex);

		// calculate square root of the the number. 
		// if our double subtract the integer is extremely small then its an integer 
		double sqrt_number = sqrt(initial_buffer_number); 
		if (sqrt_number-floor(sqrt_number) < 1e-8){
			printf("%d %d %d \n", c->consumer_id, initial_buffer_number, (int) sqrt(initial_buffer_number)); 
		}

		// we want to do a sem_post to the buffers semaphore since one thing got read off the buffer
		sem_post (&buffers);
	}
	return NULL;
}

// use message queue for the counter - Consumers coordinate together. Producers do not need to coordinate.
// may produce and may consume. 
// advanced linux book for the second part  
int sender(void* parameters){
	int i;
	struct producer_parms* p = (struct producer_parms*) parameters;

	// we produce starting at our producer id and keep going till we are less than N. 
	// We only produce multiples of P starting from our ID 
	for (i=p->producer_id; i < capacity; i+=P){

		// must have buffer space 
		sem_wait(&buffers); 

		// mutex : Queue is a critical resource 
		pthread_mutex_lock(&job_queue_mutex); 
		//printf("Hello am here on iteration %d\n", i);
		enqueue(shared_queue,i);
		//printf("Sending: %d\n",i);
		pthread_mutex_unlock(&job_queue_mutex);

		// we have started producing and another consumer may start running
		sem_post(&producers);
	}
	return NULL;
}


int main(int argc, char *argv[])
{

	int i; 
	struct timeval tv;
	double t1;
	double t2;


	// 5 args must be there 
	if (argc != 5){
		printf("Your command needs to look like: ./produce <N> <B> <P> <C>\n");
		exit(1); 
	}

	/* Initializing our N, B, P, C values*/
	capacity = atoi(argv[1]);	
	P = atoi(argv[3]); 
	C = atoi(argv[4]); 
	buffer_size = atoi(argv[2]); 

	if (buffer_size == 0 || P == 0 || C == 0){
		printf("Your buffer size, number of consumers and number of producers cannot be 0.\n"); 
		exit(1); 
	}

	// init our shared queue
	shared_queue = q(buffer_size);

	// declaring semaphores and mutex 
 	sem_init (&consumers, 0, capacity); // consumer semaphore to know when we are done 
 	sem_init (&producers, 0, 0); // set at 0 so consumers are initially blocked until a producer produces
	sem_init (&buffers, 0, buffer_size); // semaphore to make sure there is buffer space

	// we will have P prods and C consumers
	pthread_t producer_thread_id[P];
	pthread_t consumer_thread_id[C]; 

	// our time before we start
	gettimeofday(&tv, NULL);
	t1 = tv.tv_sec + tv.tv_usec/1000000.0;

	// we will produce P producers and C consumers 
	for (i=0; i < P; i++){
		struct producer_parms *pp = malloc(sizeof(struct producer_parms)); 
		pp->producer_id = i; 
		//printf("Your i value is %d", i);
		pthread_create (&producer_thread_id[i], NULL, &sender, pp);
	}

	for (i=0; i < C; i++){
		struct consumer_params *cc = malloc(sizeof(struct consumer_params)); 
		cc->consumer_id = i;
		pthread_create (&consumer_thread_id[i], NULL, &receiver, cc);
	}


	// must join it P and C times 
	for (i=0; i < P; i++){
		pthread_join (producer_thread_id[i], NULL);
	}

	for (i=0; i < C; i++){
		pthread_join (consumer_thread_id[i], NULL);
	}

	// output the total time it took
	gettimeofday(&tv, NULL);
	t2 = tv.tv_sec + tv.tv_usec/1000000.0;
	printf("System execution time: %.6lf seconds\n",t2-t1);


	return 0;
}