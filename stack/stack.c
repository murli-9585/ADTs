#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define ALLOCATION_SIZE 4 /* Allocation factor. */

void
stack_new(stack *s, int element_size) {
	assert(element_size > 0);
	s->element_size = element_size;
	s->size = 0;
	s->log_length = 0;
	s->elements = malloc(ALLOCATION_SIZE * element_size);
	assert(s->elements);
	s->log_length = ALLOCATION_SIZE;
}

bool
stack_empty(stack *s) {
	assert(s);
	return s->size == 0;
}

void
stack_dispose(stack *s) {
	if (s != NULL) {
		s->log_length = 0;
		s->size = 0;
		free(s->elements);
	}
}
void
stack_pop(stack *s, void *element) {
	assert(s);
	void *source_addr;
	source_addr = (char *)s->elements + (s->size - 1) * s->element_size;
	memcpy(element, source_addr, s->element_size);
	s->size--;
}

void
stack_push(stack *s, const void *element) {
	assert(s);
	assert(element);
	if (s->size == s->log_length) {
		void *new_elements;
		new_elements = realloc(s->elements, s->log_length+ALLOCATION_SIZE*s->element_size);
		if (new_elements == NULL) {
			printf("New elements cannot be added to stack.\n");
		}
		s->log_length += ALLOCATION_SIZE;
	}
	void *dest_addr;
	dest_addr = (char *)s->elements +s->size*s->element_size;
	memcpy(dest_addr, element, s->element_size);
	s->size++;
}
