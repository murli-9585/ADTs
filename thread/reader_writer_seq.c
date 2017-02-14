#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/* Implementing simple reader writer in a sequencial format.
   Writer gets the data into a buffer and reader reads the buffer.
   Assuming buffer length =  50.
*/

const int transmission_buf_len  = 10;
const int scoop_len = 2;
const int data_buf_len = 50;
const char *data = "123456789101112131415161718192021222324252627282930313233"
				"3435363738394041424344454647484950";

// Delay before reading or writing data.
const int delay = 1;
// Reader writer timeout.
const int timeout = 15;

/* Fills the transmission buffer given to it in
   ring buffer fashion.
*/
static void writer(char *transmission_buf, int data_len) {
	int writer_pos = 0;
	int ring_pos = 0;
	char *scoop_buf = malloc(scoop_len);
	// Read from the data buffer.
	while (data_len > writer_pos) {
		memcpy(scoop_buf, data+writer_pos, scoop_len);
		if ( scoop_buf == NULL ) {
			printf("Writer: Error scooping data.\n");
			exit(-1);
		}
		//sleep(delay);
		ring_pos = writer_pos % transmission_buf_len;
		// Write to transmission buffer.
		memcpy(transmission_buf+ring_pos, scoop_buf, scoop_len);
		writer_pos += scoop_len;
	}
	printf("Writer: overall chars:%d\n", writer_pos);
}

static void reader(char *transmission_buf, int data_len) {
	int reader_pos = 0;
	int ring_pos = 0;
	char *scoop_buf = malloc(scoop_len);
	while (data_len > reader_pos) {
		memcpy(scoop_buf, transmission_buf+ring_pos, scoop_len);
		if (scoop_buf == NULL) {
			printf("Reader: Error scooping data. \n");
			exit(-1);
		}
		printf("%s", scoop_buf);
		reader_pos += scoop_len;
		ring_pos = reader_pos % transmission_buf_len;
	}
	printf("\n");
}

static int get_data_len(void) {
	int data_len = 0;
	assert(data != NULL);
	while (data[data_len] != '\0') {
		printf("%c", data[data_len]);
		data_len++;
	}
	return data_len;
}

int main(void) {
	int data_len = get_data_len();
	printf("data len:%d\n", data_len);
	char *transmission_buf = malloc(transmission_buf_len);
	writer(transmission_buf, data_len);
	reader(transmission_buf, data_len);
	printf("All Done!!\n");
}
