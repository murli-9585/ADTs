#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/* Reader-writer implementation using semaphores.
   TODO: This should actually wait on timeout than data_len.
*/
// Header Stuff.
typedef struct {
	int transmission_buf_len;
	int scoop_len;
	int transmission_timeout;
	char *transmission_buf;
	int data_len;
	int sem_delay;
	sem_t *writer_sem;
	sem_t *reader_sem;
}transmission_info;

static void initialize_transmission_buf(transmission_info* );
static void writer(transmission_info* );
static void reader(transmission_info* );
static int get_data_len(void);
static void cleanup(transmission_info* );

const char *data = "123456789101112131415161718192021222324252627282930313233"
				"3435363738394041424344454647484950";

static void initialize_transmission_buf
		(transmission_info* t_info) {
	assert(t_info);
	t_info->transmission_buf_len = 10;
	t_info->scoop_len = 2;
	t_info->transmission_timeout = 60; //1 min timeout.
	t_info->transmission_buf = malloc(t_info->transmission_buf_len);
	t_info->data_len = get_data_len();
	t_info->sem_delay = 5; // transmission_buf_len / scoop_len
	t_info->writer_sem = sem_open
			("Writer_semaphore", O_CREAT, 0644, t_info->sem_delay);
	if (t_info->writer_sem == SEM_FAILED)
		printf("Error opening write semaphore.\n");
	t_info->reader_sem = sem_open("Reader_semaphore", O_CREAT, 0644, 0);
	if (t_info->reader_sem == SEM_FAILED)
		printf("Error opening read semaphore.\n");
}

/* Fills the transmission buffer given to it in
   ring buffer fashion.
*/
static void writer(transmission_info* t_info) {
	assert(t_info);
	int writer_pos = 0;
	int ring_pos = 0;
	char *scoop_buf = malloc(t_info->scoop_len);
	// Read from the data buffer.
	while (t_info->data_len > writer_pos) {
		memcpy(scoop_buf, data+writer_pos, t_info->scoop_len);
		if ( scoop_buf == NULL ) {
			printf("Writer: Error scooping data.\n");
			exit(-1);
		}
		ring_pos = writer_pos % t_info->transmission_buf_len;
		sem_wait(t_info->writer_sem);
		// Write to transmission buffer.
		memcpy(t_info->transmission_buf+ring_pos, scoop_buf, t_info->scoop_len);
		writer_pos += t_info->scoop_len;
        // For readability.
		printf("Wrote :%s to Transmission buf:%s\n",
            scoop_buf, t_info->transmission_buf);
        sleep(1);
		sem_post(t_info->reader_sem);
	}
	printf("Writer: overall chars:%d\n", writer_pos);
	pthread_exit(NULL);
}

static void reader(transmission_info* t_info) {
	int reader_pos = 0;
	int ring_pos = 0;
	char *scoop_buf = malloc(t_info->scoop_len);
	printf("Reader: Data length:%d\n", t_info->data_len);
	while (t_info->data_len > reader_pos) {
		sem_wait(t_info->reader_sem);
		memcpy(scoop_buf, t_info->transmission_buf+ring_pos, t_info->scoop_len);
		if (scoop_buf == NULL) {
			printf("Reader: Error scooping data. \n");
			exit(-1);
		}
		printf("Reader: %s\n", scoop_buf);
        sleep(1);
		sem_post(t_info->writer_sem);
		reader_pos += t_info->scoop_len;
		ring_pos = reader_pos % t_info->transmission_buf_len;
	}
	printf("\n");
	pthread_exit(NULL);
}

static int get_data_len(void) {
	int data_len = 0;
	assert(data != NULL);
	while (data[data_len] != '\0') {
		data_len++;
	}
	return data_len;
}

static void cleanup(transmission_info* t_info) {
	assert(t_info);
	// How to get semaphore name?
	//sem_unlink(t_info->writer_sem);
	//sem_unlink(t_info->reader_sem);
	sem_close(t_info->writer_sem);
	sem_close(t_info->reader_sem);
	free(t_info->transmission_buf);
}

int main(void) {
	pthread_attr_t thread_attr;
	pthread_t reader_thread;
	pthread_t writer_thread;
	transmission_info* t_info;

	pthread_attr_init(&thread_attr);
	t_info = malloc(sizeof(transmission_info));

	initialize_transmission_buf(t_info);
	printf("data len:%d\n", t_info->data_len);

	pthread_create(&writer_thread, &thread_attr, (void* )&writer, t_info);
	pthread_create(&reader_thread, &thread_attr, (void* )reader, t_info);

	pthread_join(writer_thread, NULL);
	pthread_join(reader_thread, NULL);

	cleanup(t_info);
	free(t_info);

	printf("All Done!!\n");
}
