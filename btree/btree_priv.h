#ifndef BTREE_PRIV_H
#define BTREE_PRIV_H

#include <stddef.h>

// Globals
typedef struct node node;
typedef struct btree_node btree_node;

/*
    Btree has btree_nodes which internally has Order(N) of nodes.
    Each node has 2 children which point to Btree_nodes (L/R).
    In a Order (N) btree node there are 2N children(keys), pointing to 2N+1
    btree_nodes.

    Order(2) Btree_node.
    btree_node
    ------------------------------------------------
    |   ______     ______     ______     ______     |
    |  |node1 |   |node2 |   |node3 |   |node4 |    |
    |  |______|-->|______|-->|______|-->|______|    |
    |  /      \   /      \   /      \   /       \   |
    --/--------\-/------- \-/--------\-/---------\---
    btree_node btree_node....
    */

struct btree_node {
        node *root; /* points to the first in group of linked nodes. */
        size_t item_count; /* Number of node items. */
        size_t order; /* Defines the order of the btree node. */
        size_t key_size;
        btree_node *parent;
        int (*compare)(const void *, const void *);
};

struct node{
        void *data;
        node *next;
        btree_node *left;
        btree_node *right;
        btree_node *parent_bt; // The btree node of the node(s).
};

static inline node *
root(btree_node *bt) {
        return bt->root;
}

static inline btree_node *
parent(btree_node *bt) {
        return bt->parent;
}

static inline size_t
item_count(btree_node *bt) {
        return bt->item_count;
}

static inline size_t
bt_order(btree_node *bt) {
        return bt->order;
}

static inline size_t
key_size(btree_node *bt) {
        return bt->key_size;
}

static inline void *
data(node *item) {
        return item->data;
}

static inline btree_node *
right(node *item) {
        return item->right;
}

static inline btree_node *
left(node *item) {
        return item->left;
}

static inline node *
next(node *item) {
        return item->next;
}

static inline btree_node *
get_btreenode(node *item) {
        return item->parent_bt;
}

/* This uses int input for now.*/
static inline int
cmp(const void *src, const void *dest) {
        return *(const int *)src - *(const int *)dest;
}

static inline int
data_compare(btree_node *bt, const void *src, const void *dest) {
        return (bt->compare)(src, dest);
}

#endif /*btree_priv.h */
