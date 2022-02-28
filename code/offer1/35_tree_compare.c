#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 描述: 
 子树比较
 */

struct tree {
    int value;
    struct tree * left;
    struct tree * right;
};

static bool tree_compare_value(struct tree * root1, struct tree * root2) {
    if (root2 == NULL) {
        return true;
    }
    if (root1 == NULL) {
        return false;
    }
    if (root1->value != root2->value) {
        return false;
    }
    
    if (tree_compare_value(root1->left, root2->left)) {
        return tree_compare_value(root1->right, root2->right);
    }
    return false;
}

static bool tree_compare_partial(struct tree * root1, struct tree * root2) {
    if (root1->value == root2->value) {
        if (tree_compare_value(root1, root2)) {
            return true;
        }
    }

    if (root1->left) {
        if (tree_compare_partial(root1->left, root2)) {
            return true;
        }
    }

    if (root1->right) {
        return tree_compare_partial(root1->right, root2);
    }

    return false;
}

bool tree_compare(struct tree * root1, struct tree * root2) {
    if (root2 == NULL) {
        return true;
    }

    if (root1 == NULL) {
        return false;
    }

    return tree_compare_partial(root1, root2);
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 35_tree_compare 35_tree_compare.c
//
int main(void) {

    exit(EXIT_SUCCESS);
}
