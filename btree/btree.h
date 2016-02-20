/* Binary Tree (Order N) implementation. */

#ifndef BTREE_H
#define BTREE_H
#include <stddef.h>
#include "btree_priv.h"

// TODO: Change public names with Btree prefix.
extern int btree_init(btree_node **tree, size_t order, size_t key_size,
        int (*comparer)(const void *, const void *));
extern int btree_search(btree_node *tree, const void *key);
extern int btree_minimum(btree_node *tree, void *value);
extern int btree_maximum(btree_node *tree, void *value);
extern int btree_insert(btree_node **tree, void *value);
extern int btree_delete(btree_node *tree, void *value);
extern void btree_print(btree_node *tree);
extern int btree_height(btree_node *tree);

#endif  /* BTREE_H */

