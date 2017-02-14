/*
   Simple air line ticket selling example using posix threads.
*/
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_TICKETS_AVALIABLE 25
#define NUM_AGENTS 5

struct ticket_info {
        int num_tickets;
        sem_t *semaphore;
};

static void sell_tickets(void *);

int main() {
        const char* sem_name = "/semaphore";
        pthread_t threads[NUM_AGENTS];
        pthread_attr_t thread_attr;
        sem_t *semaphore;
        int rc;
		struct ticket_info ticket;
        pthread_attr_init(&thread_attr);
        int num_tickets = NUM_TICKETS_AVALIABLE;
		// With last argument, shared among # of threads, 1 is a mutex.
		semaphore = sem_open(sem_name, O_CREAT, 0644, 1);
		if (semaphore == SEM_FAILED) {
				printf("Error opening semaphore.Error:%d\n", *semaphore);
				exit(-1);
		}

		// Shared data among threads.
        ticket.num_tickets = num_tickets;
		ticket.semaphore = semaphore;

		// Start Threads.
        for (int i = 0; i<NUM_AGENTS; i++) {
                rc = pthread_create(&threads[i], &thread_attr,
                               &sell_tickets, (void *) &ticket);
                if (rc) {
                    printf("ERROR: Return code:%d from pthread_create", rc);
                }
        }

		// Better join as main thread might exit here.
        //pthread_exit(NULL);
		for (int i=0; i<NUM_AGENTS; i++)
				pthread_join(threads[i], NULL);

		if (sem_unlink(sem_name) == -1)
			printf("Failed unlinking semaphore:%s\n", sem_name);
		sem_close(semaphore);
		printf("All Done!!\n");
}

static void sell_tickets(void *ticket_data) {
		int tickets_avaliable = 1;
		struct ticket_info *ticket = (struct ticket_info *)ticket_data;
        while (tickets_avaliable) {
			   sem_wait(ticket->semaphore);
               
			   if (ticket->num_tickets == 0) {
						tickets_avaliable = 0;
						printf("Sold out!\n");
			   }
			   // This is just a test. Doomed we if get here.
			   else if (ticket->num_tickets < 0 )
                        printf("WHOOO!!, Tickets Oversold! \n");
			   else {
               			printf("Sold ticket: %d\n",
                      	ticket->num_tickets);
               			ticket->num_tickets--;
			   }

			   sem_post(ticket->semaphore);
        }
        pthread_exit(NULL);
}
