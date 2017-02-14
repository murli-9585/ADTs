/*
   Simple air line ticket selling example using posix threads.
   Each thread selling exactly the same amount of tickets.
   ref:https://computing.llnl.gov/tutorials/pthreads/#Thread
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_TICKETS_AVALIABLE 25
#define NUM_AGENTS 5
/*struct ticket_info {
        int agent_id;
        int num_tickets_to_sell;
};*/
static void sell_tickets(void *);

int main() {

        pthread_t threads[NUM_AGENTS];
        pthread_attr_t thread_attr;
        int rc;
        //struct ticket_info ticket;
        pthread_attr_init(&thread_attr);
        int num_tickets = NUM_TICKETS_AVALIABLE;
        // Start Threads.
        for (int i = 0; i<NUM_AGENTS; i++) {
                rc = pthread_create(&threads[i], &thread_attr,
                               &sell_tickets, (void *) &num_tickets);
                if (rc) {
                    printf("ERROR: Return code:%d from pthread_create", rc);
                }
        }
        pthread_exit(NULL);
        printf("All Done!\n");
}

static void sell_tickets(void *num_tickets) {

        while (1) {
               if ( *(int *)num_tickets <= 0) {
                       if( *(int *)num_tickets < 0)
                               printf("WHOOO!!!: SOLD MORE TICKETS \n");
                       break;
               }
               printf("Sold ticket: %d\n",
                      *(int *)num_tickets);
               (*(int *)num_tickets)--;
        }
        pthread_exit(NULL);
}
