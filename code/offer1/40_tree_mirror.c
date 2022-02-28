#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "stack.h"

/*
 描述: 
 二叉树镜像
 */

struct tree {
    int value;
    struct tree * left;
    struct tree * right;
};

void tree_mirror_image(struct tree * root) {
    if (root == NULL || (root->left == NULL && root->right == NULL)) {
        return;
    }

    struct tree * temp = root->left;
    root->left = root->right;
    root->right = temp;

    tree_mirror_image(root->left);
    tree_mirror_image(root->right);
}

void tree_mirror_image_non_recursive(struct tree * root) {
    if (root == NULL || (root->left == NULL && root->right == NULL)) {
        return;
    }

    struct stack s; stack_init(&s);

    stack_push(&s, root);
    do {
        struct tree * node = stack_pop_top(&s);
        struct tree * temp = node->left;
        node->left = node->right;
        node->right = temp;

        if (node->left) {
            stack_push(&s, node->left);
        }
        if (node->right) {
            stack_push(&s, node->right);
        }
    } while (stack_exist(&s));

    stack_free(&s);
}

void tree_inorder(struct tree * root) {
    if (root == NULL) {
        return;
    }

    printf("tree_inorder: ");

    struct stack s; stack_init(&s);

    do {
        while (root) {
            stack_push(&s, root);
            root = root->left;
        }

        root = stack_pop_top(&s);
        printf(" %d", root->value);

        root = root->right;
    } while (!stack_empty(&s) || root != NULL);

    stack_free(&s);

    printf("\n");
}

inline struct tree * tree_new(int value) {
    struct tree * node = malloc(sizeof(struct tree));
    node->value = value;
    node->left = node->right = NULL;
    return node;
}

struct tree * tree_create(int preorder[], int inorder[], int len) {
    if (preorder == NULL || inorder == NULL || len <= 0) {
        return NULL;
    }

    if (len == 1) {
        return tree_new(preorder[0]);
    }

    int value = preorder[0];
    
    // 找到中序遍历中分割点
    int index = 0;
    while (inorder[index] != value) 
        index++;

    // 辅助调试
    // printf("index = %d, value = %d, len=%d, len-index-1 = %d\n", index, value, len, len-index-1);

    struct tree * root = tree_new(value);
    root->left = tree_create(preorder+1, inorder, index);
    root->right = tree_create(preorder+1+index, inorder+index+1, len-index-1);
    return root;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 40_tree_mirror 40_tree_mirror.c
//
int main(void) {

/*
            1
          /   \
         2     3
        /     / \
       4     5   6
        \       /
        7      8
 */

    int preorder[] = { 1, 2, 4, 7, 3, 5, 6, 8 };
    int inorder[] = { 4, 7, 2, 1, 5, 3, 8, 6 };
    int len = sizeof(preorder) / sizeof(int);

    assert(sizeof(preorder) == sizeof(inorder));
    
    struct tree * root = tree_create(preorder, inorder, len);
    tree_inorder(root);

    tree_mirror_image(root);

    tree_inorder(root);

    tree_mirror_image_non_recursive(root);

    tree_inorder(root);

    exit(EXIT_SUCCESS);
}
