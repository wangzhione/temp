#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 

   二叉树深度

   平衡二叉树判断
 */

struct tree {
    int value;
    struct tree * left;
    struct tree * right;
};

int tree_depth(struct tree * tree) {
    if (tree == NULL) {
        return 0;
    }

    int left = tree_depth(tree->left);
    int right = tree_depth(tree->right);
    return left >= right ? left+1 : right+1;
}

bool tree_is_balacne(struct tree * tree) {
    if (tree == NULL) {
        return true;
    }

    int left = tree_depth(tree->left);
    int right = tree_depth(tree->right);

    int diff = left - right;
    if (diff < -1 || diff > 1) {
        return false;
    }

    return tree_is_balacne(tree->left) && tree_is_balacne(tree->right);
}

bool tree_is_balance_optimize_partial(struct tree * tree, int * depth) {
    if (tree == NULL) {
        *depth = 0;
        return true;
    }

    // 左 -> 右 -> 中
    int left;
    if (!tree_is_balacne_optimize_partial(tree->left, &left)) {
        return false;
    }
    int right;
    if (!tree_is_balance_optimize_partial(tree->right, &right)) {
        return false;
    }

    int diff = left - right;
    if (diff >= -1 && diff <= 1) {
        *depth = left > right ? left + 1 : right + 1;
        return true;
    }
    return false;
}

bool tree_is_balance_optimize(struct tree * tree) {
    if (tree == NULL) {
        return false;
    }

    int depth;
    return tree_is_balance_optimize_partial(tree, &depth);
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 61_tree_depth 61_tree_depth.c
//
int main(void) {

    exit(EXIT_SUCCESS);
}
