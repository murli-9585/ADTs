/* B-tree Order(n) implemenation. */

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

/* Static method decleration.*/
static node *
create_node(size_t key_size, void *key);
static void
print_node(node *item);
static void
print_btreenode(btree_node *bt_node);
static btree_node *
create_btreenode(node *item, size_t order, size_t key_size);
static int
search_in_btreenode(btree_node *btree, const void *key, node **ret);
static int
locate_key(btree_node *btree, const void *key, btree_node **dest_node);
static void
update_node_children_parent(node *item, btree_node *dest_btnode);
static void
insert_item_in_btnode(btree_node *dest_btnode, node *item);
static node *
split_up(btree_node *bt_node, node *item);
static void
insert_item_in_tree(btree_node **tree, btree_node *dest_btnode, node *item);
static void
node_data(node *node_ref, void *ret_key);
static void
print_node(node *item);
/* End static method decleration */

/* static methods. */
/* Finds the key with-in a bt-node's root. Returns
   -1 if the key to be found is less than the last node it searched.
   0 on success
   1 if the key to be found is on the right of the last node it searched.
   ret has the last item it searched within the root.
*/
static int
search_in_btreenode(btree_node *btree, const void *key, node **ret) {

		assert(btree);

		node *item = root(btree);
		int result;

		while(item) {
				*ret = item;
				result = data_compare(btree, key, data(item));

				if (result <= 0) // Not in this btree node, return.
						break;
				else
						item = item->next;
		}
		return result;
}

/* Use Btree terminology when considering whole or sub-tree. Btreenode when
 * considering only the Btree node.
*/
static int
locate_key(btree_node *btree, const void *key, btree_node **dest_node) {

		int btnode_search_val;
		node *ret;

		while (btree) {
				*dest_node = btree;
				btnode_search_val = search_in_btreenode(btree, key, &ret);

				if (btnode_search_val == 0)
						break;
				else if (btnode_search_val < 0)
						btree = ret->left;
				else
						btree = ret->right;
		}
		// 0 on success 1 on failure to find node with same key.
		return btnode_search_val == 0 ? 0 : 1;
}

static void
update_node_children_parent(node *item, btree_node *dest_btnode) {

		assert(dest_btnode);
		assert(item);
		node *item_ref = item;
		while (item_ref) {
			if (left(item_ref))
					left(item_ref)->parent = dest_btnode;
			if (right(item_ref))
					right(item_ref)->parent = dest_btnode;
			item_ref = next(item_ref);
		}
}

/*
 * Insert a node item into btree node. This currently supports adding a node
 * that is split up or a inserting new item to the existing btree node.
 */
static void
insert_item_in_btnode(btree_node *dest_btnode, node *item) {

		node *current_item = root(dest_btnode);
		node *prev_item = NULL;

		while (current_item &&
			  (data_compare(dest_btnode, data(item), data(current_item))>0)) {
				prev_item = current_item;
				current_item = current_item->next;
		}
		item->next = current_item;

		if (prev_item == NULL && current_item) { // Insert in front.
				dest_btnode->root = item;
				current_item->left = item->right;
		}
		else if (prev_item && current_item) { // Insert between existing items.
				prev_item->next = item;
				prev_item->right = item->left;
				current_item->left = item->right;
		}
		else if (current_item == NULL && prev_item) { // Insert at end.
				prev_item->next = item;
				prev_item->right = item->left;
		}
		else
				assert("Unknown location to insert.\n");

		item->parent_bt = dest_btnode;
		dest_btnode->item_count += 1;
		update_node_children_parent(item, dest_btnode);
}

/*
 * Splits up the bt_node in middle after inserting the item into bt_node.
 * Usually the left-from-middle portion will stay as-is and right-from-middle
 * will be spilt to a new btree node with middle item as root.
 */
static node *
split_up(btree_node *bt_node, node *item) {

		unsigned int split_pos = 0;
		node *prev, *next, *new_root;
		prev = next = new_root = NULL;
		unsigned int original_count = item_count(bt_node);

		insert_item_in_btnode(bt_node, item);
		new_root = root(bt_node);
		/*
		Node was added before splitting so floor of original items by 2
		will be split position.
		*/
		unsigned int middle_item = floor(original_count/2);

		while (split_pos < middle_item) {
				prev = new_root;
				new_root = new_root->next;
				split_pos++;
		}
		next = new_root->next;
		new_root->next = NULL;

		/* bt_node becomes the left btree and right_bt as right btree
		   of root node (new_root). Root node will be inserted into its parent.
		*/
		prev->next = NULL;
		bt_node->item_count = split_pos;
		new_root->left = bt_node;

		// Create new bt_node for next nodes.
		btree_node *right_bt = create_btreenode(next, bt_order(bt_node),
				key_size(bt_node));
		right_bt->root = next;
		new_root->right = right_bt;
		assert(item_count(right_bt) == (original_count-split_pos));

		return new_root;
}

/*
 * Inserts the node into btree. It can change the tree height if needed.
 */
static void
insert_item_in_tree(btree_node **tree, btree_node *dest_btnode, node *item) {

		// When the split happens at the head of the tree.
		if (!dest_btnode) {
				btree_node *bt_node = create_btreenode(item, bt_order(*tree),
							sizeof(data(item)));
				update_node_children_parent(item, bt_node);
				// Make it the head of the tree.
				*tree = bt_node;
				return;
		}

		int item_diff = (int)(item_count(dest_btnode) -
				bt_order(dest_btnode));
		if (item_diff < 0) {
				insert_item_in_btnode(dest_btnode, item);
		}
		else if (item_diff == 0) {
				node *new_item;
				btree_node *new_dest = parent(dest_btnode);
				new_item = split_up(dest_btnode, item);

				/* This will lead to recursion. When parent of new_root is
				   null then it reached the head of the tree.
				*/
				insert_item_in_tree(tree, new_dest, new_item);
		}
		else {
				assert("Unexpected node count in btree node.");
		}
}

static node *
create_node(size_t key_size, void *key) {

		node *item = malloc(sizeof(node));
		assert(item);
		item->data = malloc(sizeof(key_size));
		assert(item->data);

		memcpy(item->data, key, key_size);
		item->left = item->right = item->parent_bt = NULL;
		item->next = NULL;
		return item;
}

/* Creates a Btree node with item as root(first) element in the btree node. */
static btree_node *
create_btreenode(node *item, size_t order, size_t key_size) {

		btree_node *bt_node = malloc(sizeof(btree_node));
		assert(bt_node);

		bt_node->root = item;
		node *temp_item = item;
		bt_node->item_count = 0;

		// Calcuate item_count of new btree node.
		while (temp_item) {
				bt_node->item_count++;
				temp_item->parent_bt = bt_node;
				temp_item = temp_item->next;
		}

		// Apply btree_node setting.
		bt_node->order = order;
		bt_node->parent = NULL;
		bt_node->key_size = key_size;
	    bt_node->compare = cmp;

		if (item)
				update_node_children_parent(item, bt_node);

		return bt_node;
}

static void
node_data(node *node_ref, void *ret_key) {
		void *start_addr = (char *)data(node_ref);
		size_t size = key_size(get_btreenode(node_ref));
		memcpy(ret_key, start_addr, size);
}

/* Ideal solution is to use Queue, for now simple hack!
   It would print all the nodes in Btree node and then travel
   left BT and right BT.
 */
static void
print_btreenode(btree_node *bt_node) {

		if (!bt_node) {
				printf("NULL\n");
		}
		else {
				int value;
				node *node_ref1 = root(bt_node);
				node *node_ref = node_ref1;

				// Print all node data in btree node.
				while (node_ref1) {
						node_data(node_ref1, &value);
						printf("| %d ", value);
						node_ref1 = next(node_ref1);
				}

				printf("|");
				printf("\n");

				// Now print children.
				while (node_ref) {
						print_node(node_ref);
						node_ref = node_ref->next;
				}
		}
}

static void
print_node(node *item) {
		/* Since left and right represent same for middle items, validate it
		   and print accordingly.
		*/
		if (item) {
				if (item->left != item->right) {
						print_btreenode(left(item));
						print_btreenode(right(item));
				}
		}
		else
				print_btreenode(left(item));
}
/*  End static methods. */

int
btree_init(btree_node **tree, size_t order, size_t key_size,
		int (*comparer)(const void *, const void *)) {

		*tree = NULL;
		assert(order > 0);
		assert(key_size > 0);

		*tree = create_btreenode(NULL, order, key_size);
		(*tree)->compare = comparer ? comparer : cmp;
		return 0;
}

int
btree_minimum(btree_node *tree, void *value) {

		if (!tree || !root(tree))
				return EINVAL;

		node *item = root(tree);
		while (left(item) != NULL)
				item = root(left(item));

		node_data(item, value);
		return 0;
}

int
btree_maximum(btree_node *tree, void *value) {

		if (!tree || !root(tree))
				return EINVAL;

		node *item = root(tree);
		while (item != NULL) {
				if (next(item) == NULL) {
						if (right(item) != NULL)
								item = root(right(item));
						else
								break;
				}
				else
						item = next(item);
		}

		node_data(item, value);
		return 0;
}

int
btree_search(btree_node *btree, const void *key) {

		assert(btree);
		btree_node *bt_node = NULL;
		return locate_key(btree, key, &bt_node);
}

int
btree_insert(btree_node **tree, void *key) {

		assert(*tree);
		assert(key);
		int retval = 0;
		btree_node *tree_ref = *tree;

		/* Validate if its the root item of btree_node */
		assert(!parent(tree_ref));
		if(!root(tree_ref)) {
				node *root_item = create_node(key_size(tree_ref), key);
				root_item->parent_bt = tree_ref;
				tree_ref->root = root_item;
				tree_ref->item_count++;
		}
		else {
				int result;
				btree_node *dest_btnode = NULL;
				if ((result = locate_key(tree_ref, key, &dest_btnode)) == 0)
						return retval = 1;
				else {
						node *item = create_node(key_size(tree_ref), key);
						insert_item_in_tree(tree, dest_btnode, item);
				}
		}
		return retval;
}

int
btree_height(btree_node *bt_node) {
		assert(bt_node);

		unsigned int height = 0;
		btree_node *bt_ref = bt_node;

		//This is balanced, left or right path should do.
		node *node_ref = root(bt_ref);
		while ((bt_ref = left(node_ref)) != NULL) {
				node_ref = root(bt_ref);
				height++;
		}
		return height;
}

void
btree_print(btree_node *bt_node) {
		print_btreenode(bt_node);
}
