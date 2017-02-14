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
struct ticket_info {
        int agent_id;
        int num_tickets_to_sell;
};
static void sell_tickets(void *);

int main() {

        pthread_t threads[NUM_AGENTS];
        pthread_attr_t thread_attr;
        int rc;
        struct ticket_info ticket[NUM_AGENTS];
        pthread_attr_init(&thread_attr);
        /* This code assigns each thread their own data copy, ticket[i]
           Which makes it clear view of what each thread owns. Does not defines
           critical section.
        */
        for (int i=0; i<NUM_AGENTS; i++) {
                ticket[i].agent_id = i;
                ticket[i].num_tickets_to_sell = NUM_TICKETS_AVALIABLE / NUM_AGENTS;
                rc = pthread_create(&threads[i], &thread_attr,
                               &sell_tickets, (void *) &ticket[i]);
                if (rc) {
                    printf("ERROR: Return code:%d from pthread_create", rc);
                }
        }
        pthread_exit(NULL);
        printf("All Done!\n");
}

static void sell_tickets(void * ticket_data) {
        struct ticket_info *ticket = (struct ticket_info *)ticket_data;
        while (1) {
               if (ticket->num_tickets_to_sell == 0) {
                       break;
               }
               sleep(2);
               printf("Agent id: %d, sold ticket: %d\n",
                      ticket->agent_id, ticket->num_tickets_to_sell);
               ticket->num_tickets_to_sell--;
        }
        printf("Agent_id: %d, Done!\n", ticket->agent_id);
        pthread_exit(NULL);
}
