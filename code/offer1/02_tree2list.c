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

static inline void tree_print(struct tree * node) {
    printf(" %d ", node->value);
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

void tree_postorder(struct tree * root) {
    if (root) {
        tree_postorder(root->left);
        tree_postorder(root->right);
        tree_print(root);
    }
}

void tree_delete(struct tree * root) {
    if (root == NULL) {
        return;
    }
    
    printf("postorder : ");

    struct tree * pre = NULL;
    struct stack s; stack_init(&s);

    stack_push(&s, root);
    do {
        struct tree * cur = stack_top(&s);
        if ((cur->left == NULL && cur->right == NULL) 
        || ((cur->left == pre || cur->right == pre) && pre != NULL)) {
            pre = cur;
            
            // 打印计数
            tree_print(pre);

            stack_popped(&s);
            tree_free(cur);
        } else {
            if (cur->right) {
                stack_push(&s, cur->right);
            }
            if (cur->left) {
                stack_push(&s, cur->left);
            }
        }
    } while(!stack_empty(&s));

    stack_free(&s);

    printf("\n");
}

void tree_list_partial(struct tree * root, struct tree ** pprev) {
    if (root == NULL) {
        return;
    }

    // 左
    tree_list_partial(root->left, pprev);

    // 中
    // 当前结点的左指向前一个
    root->left = *pprev;
    if (*pprev) {
        (*pprev)->right = root;
    }
    *pprev = root;

    // 右
    tree_list_partial(root->right, pprev);
}

struct tree * tree_list(struct tree * root) {
    if (root == NULL || (root->left == NULL && root->right == NULL)) {
        return root;
    }

    struct tree * prev = NULL;
    tree_list_partial(root, &prev);

    // 找到起始位置
    while (root->left) {
        root = root->left;
    }
    return root;
}

static void tree_list_non_recursive_partial(struct tree * root, struct stack * s) {
    struct tree * node;
    struct tree * prev = NULL;

    do {
        // 找到最左结点
        while (root) {
            stack_push(s, root);
            root = root->left;
        }

        // 根结点处理
        node = stack_pop(s);
        node->left = prev;
        if (prev) {
            prev->right = node;
        }
        prev = node;

        // 切换到右子树处理
        root = node->right;
    } while (!stack_empty(s) || root != NULL);
}

struct tree * tree_list_non_recursive(struct tree * root) {
    if (root == NULL || (root->left == NULL && root->right == NULL)) {
        return root;
    }

    struct stack s; stack_init(&s);

    tree_list_non_recursive_partial(root, &s);

    stack_free(&s);

    while (root->left) {
        root = root->left;
    }

    return root;
}

//
// build: gcc -g -O3 -Wall -Wextra -Werror -o 02_tree2list 02_tree2list.c
//
int main(void) {
    struct tree * root = tree_create();

    printf("postorder : ");
    tree_postorder(root);
    printf("\n");

    tree_delete(root);

    root = tree_create();

    root = tree_list(root);

    printf(" inorder  : ");
    while (root) {
        struct tree * next = root->right;
        tree_print(root);
        tree_free(root);
        root = next;
    }
    printf("\n");

    root = tree_create();

    root = tree_list_non_recursive(root);

    printf(" inorder  : ");
    while (root) {
        struct tree * next = root->right;
        tree_print(root);
        tree_free(root);
        root = next;
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}