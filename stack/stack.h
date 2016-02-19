#include <stdbool.h>
typedef struct {
		int log_length; /* Overall log size. */
		int element_size;
		int size; /* Current Stack size. */
		void *elements;
} stack;


void stack_new(stack *s, int element_size);
void stack_push(stack *s, const void *element);
void stack_pop(stack *s, void *element);
bool stack_empty(stack *s);
void stack_dispose(stack *s);
