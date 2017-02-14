/*
   Simple air line ticket selling example in sequential format.
*/
#include <stdio.h>

#define NUM_TICKETS_AVALIABLE 25
#define NUM_AGENTS 5
static void sell_tickets(int, int *);

int main() {

        int tickets_per_agent;
        for (int i=0; i<NUM_AGENTS; i++) {
                tickets_per_agent = NUM_TICKETS_AVALIABLE / NUM_AGENTS;
                sell_tickets(i, &tickets_per_agent);
        }
        printf("All Done!\n");
}

static void sell_tickets(int agent_id, int *tickets_to_sell) {
        while (1) {
               if (*tickets_to_sell == 0)
                       break;

               printf("Agent id: %d, sold ticket: %d\n",
                   agent_id, *tickets_to_sell);
               (*tickets_to_sell)--;
        }
        printf("Agent_id: %d, Done!\n", agent_id);
}
