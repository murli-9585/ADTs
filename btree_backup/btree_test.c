#include "btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

btree_node *
test_insert(int val, int order) {
		int i;
		btree_node *btree = NULL;
		btree_init(&btree, order, sizeof(int), NULL);
		for (i=0;i<val;i++)
				btree_insert(&btree, &i);
		return btree;
}

void
print_tree(btree_node *btree, int everything) {
		btree_print(btree);

		if (everything) {
				int min, max;
				int search_item = 25;
				btree_minimum(btree, &min);
				btree_maximum(btree, &max);
				printf("Minimum: %d\n", min);
				printf("Maximum: %d\n", max);
				printf("Height: %d\n", btree_height(btree));
				printf("Search 25, result: %d", btree_search(btree, &search_item));
		}
}

void
help() {
		printf("Inserts btree items from 0..n in btree of order N.\n"\
				" ./btree_test <num_items_to_insert> <order> \n\n");
		exit(0);
}

int
main(int argc, char** argv) {
		if (argc == 2 || argc == 3) {
				int  num_inserts = 0;
				int order = 0;
				char *end;
				num_inserts = strtol(argv[1], &end, 10);
				if (argc ==3)
						order = strtol(argv[2], &end, 10);
				btree_node *tree = test_insert(num_inserts, order > 0 ? order : 3);
				print_tree(tree, 1);
		}
		else
				help();
		return 0;
}

