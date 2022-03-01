#include "q.h"
#include "stack.h"

struct tree {
    struct tree * left;
    struct tree * right;

    int value;
};

static inline struct tree * tree_new(int value) {
    struct tree * node = malloc(sizeof(struct tree));
    node->left = node->right = NULL;
    node->value = value;
    return node;
}

static inline void tree_free(struct tree * node) {
    free(node);
}

static struct tree * tree_create(void) {
	// 1 2 3 4 5 6 7 8 9
	/*
				5
			   /  \
			  3    7
			 / \   /\
			2   4 6  8
		   /          \
		  1            9
	*/

    int i;
    struct tree * trees[9];
    for (i = 0; i < 9; i++) {
        trees[i] = tree_new(i+1);
    }

    trees[4]->left = trees[2];
    trees[4]->right = trees[6];

    trees[2]->left = trees[1];
    trees[2]->right = trees[3];

    trees[1]->left = trees[0];

    trees[6]->left = trees[5];
    trees[6]->right = trees[7];

    trees[7]->right = trees[8];

    return trees[4];
}

void tree_layer_order(struct tree * root) {
    if (root == NULL) {
        return;
    }

    printf("tree_layer_order:\n");

    q_t q; q_init(q);

    q_push(q, root);
    do {
        struct tree * node = q_pop(q);

        printf("%d ", node->value);

        if (node->left) {
            q_push(q, node->left);
        }
        if (node->right) {
            q_push(q, node->right);
        }
    } while (q_exist(q));

    q_free(q);

    printf("\n");
}

bool tree_in_tailorder(struct tree * root, int * vector, int len) {
    if (vector == NULL || len <= 0) {
        return true;
    }
    if (root == NULL) {
        return false;
    }

    int n = 0;
    bool res = true;

    struct tree * prev = NULL;
    struct stack s; stack_init(&s);
    stack_push(&s, root);
    do {
        root = stack_top(&s);
        if ((root->left == NULL && root->right == NULL) 
        || ((prev != NULL) && (root->left == prev || root->right == prev))) {
            printf(" (%d - %d) ", root->value, vector[n]);

            if (n >= len || root->value != vector[n++]) {
                res = false;
                break;
            }

            prev = root;
            stack_popped(&s);
        } else {
            if (root->right != NULL) {
                stack_push(&s, root->right);
            }
            if (root->left != NULL) {
                stack_push(&s, root->left);
            }
        }
    } while (stack_exist(&s));
    stack_free(&s);

    return res;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 43_tree_printf 43_tree_printf.c q.c
//
int main(void) {
    struct tree * root = tree_create();

    // 按照层次遍历
    tree_layer_order(root);

	/*
				5
			   /  \
			  3    7
			 / \   /\
			2   4 6  8
		   /          \
		  1            9
	*/
    int vector[] = { 1, 2, 4, 3, 6, 9, 8, 7, 5 };
    int len = sizeof vector / sizeof *vector;

    bool res = tree_in_tailorder(root, vector, len);

    printf("res = %d\n", res);

    // 释放留给操作系统
    exit(EXIT_SUCCESS);
}
