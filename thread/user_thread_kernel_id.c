#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>

int main(void) {
    pthread_t tid = pthread_self();
    pthread_id_np_t np_t_tid = pthread_getunique_np(&tid, &np_t_tid);
    printf("Posix thread_id:%d, np_t id of it:%d", tid, np_t_tid);
    return 0;
}

