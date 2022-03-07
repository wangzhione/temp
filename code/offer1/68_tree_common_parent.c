#include "stack.h"

/* describe : 

   树中两个结点的最低公共祖先.
   求树中两个结点的最低公共祖先, 此树不是二叉树, 并且没有指向父结点的指针.

            A
          /   \
         B     C
       /   \
      D     E
     / \   /|\
    F   G H I J
 */

struct tree {
    int value;

    int len;
    struct tree * children[];
};

inline struct tree * tree_new(int value, int len) {
    assert(len >= 0);
    struct tree * node = calloc(1, sizeof(struct tree) + len*sizeof(struct tree *));
    node->value = value;
    node->len = len;
    return node;
}

inline void tree_free(struct tree * node) {
    free(node);
}

/*
            A
          /   \
         B     C
       /   \
      D     E
     / \   /|\
    F   G H I J
 */
static struct tree * tree_create(struct tree * node[static 10]) {
    for (int i = 0; i < 10; i++) {
        node[i] = tree_new('A' + i, 3);
    }

    node[0]->children[0] = node[1];
    node[0]->children[1] = node[2];

    node[1]->children[0] = node[3];
    node[1]->children[1] = node[4];

    node[3]->children[0] = node[5];
    node[3]->children[1] = node[6];

    node[4]->children[0] = node[7];
    node[4]->children[1] = node[8];
    node[4]->children[2] = node[9];

    return node[0]; 
}

bool tree_postorder_compare(struct tree * node, struct tree * prev) {
    int i;

    for (i = 0; i < node->len; i++) {
        if (node->children[i] != NULL) {
            break;
        }
    }
    if (i == node->len) {
        return true;
    }

    if (prev != NULL) {
        for (i = 0; i < node->len; i++) {
            if (node->children[i] == prev) {
                return true;
            }
        }
    }
    return false;
}

/*
            A
          /   \
         B     C
       /   \
      D     E
     / \   /|\
    F   G H I J

postorder : F G D H I J E B C A
 */
void tree_delete(struct tree * tree) {
    if (tree == NULL) {
        return;
    }

    printf("tree_delete debug :\n");

    struct stack  s; stack_init(&s);
    stack_push(&s, tree);

    struct tree * prev = NULL;
    struct tree * node;
    do {
        node = stack_top(&s);
        // printf("stack_top %c, %d \n", node->value, tree_postorder_compare(node, prev));
        // getchar();
        if (tree_postorder_compare(node, prev)) {
            // debug info
            printf(" %c ", node->value);

            stack_popped(&s);
            prev = node;
            free(node);
        } else {
            for (int i = node->len - 1; i >= 0; i--) {
                if (node->children[i] != NULL) {
                    stack_push(&s, node->children[i]);
                }
            }
        }
    } while (stack_exist(&s));

    stack_free(&s);

    printf("\n");
}

/*
            A
          /   \
         B     C
       /   \
      D     E
     / \   /|\
    F   G H I J

postorder : F G D H I J E B C A
 */
bool tree_get_path(struct tree * tree, struct tree * node, struct stack * s) {
    if (tree == node) {
        return true;
    }

    stack_push(s, tree);

    bool found = false;

    for (int i = 0; !found && i < tree->len; i++) {
        if (tree->children[i] != NULL) {
            found = tree_get_path(tree->children[i], node, s);
        }
    }

    if (!found) {
        stack_popped(s);
    }
    return found;
}

struct tree * tree_common_parent(struct tree * tree, struct tree * node1, struct tree * node2) {
    if (tree == NULL) {
        return NULL;
    }
    if (node1 == NULL) {
        return node2;
    }
    if (node2 == NULL) {
        return node1;
    }

    struct stack s1; stack_init(&s1);
    bool found = tree_get_path(tree, node1, &s1);
    if (!found) {
        stack_free(&s1);
        return NULL;
    }

    struct stack s2; stack_init(&s2);
    found = tree_get_path(tree, node2, &s2);
    if (!found) {
        stack_free(&s1);
        stack_free(&s2);
        return NULL;
    }

    int s1len = stack_len(&s1);
    int s2len = stack_len(&s2);
    printf("tree_common_parent 3 s1len = %d, s2len = %d\n", s1len, s2len);
    if (s1len > s2len) {
        int diff = s1len - s2len;
        while (--diff >= 0) {
            stack_popped(&s1);
        }
    } else if (s2len > s1len) {
        int diff = s2len - s1len;
        while (--diff >= 0) {
            stack_popped(&s2);
        }
    }

    // 开始从下往上找
    while (stack_exist(&s1)) {
        struct tree * parent1 = stack_pop(&s1);
        struct tree * parent2 = stack_pop(&s2);
        if (parent1 == parent2) {
            stack_free(&s1);
            stack_free(&s2);
            return parent1;
        }
    }

    stack_free(&s1);
    stack_free(&s2);
    return NULL;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 68_tree_common_parent 68_tree_common_parent.c
//
int main(void) {
/*
            A
          /   \
         B     C
       /   \
      D     E
     / \   /|\
    F   G H I J
 */
    struct tree * node[10];
    struct tree * tree = tree_create(node);

    struct tree * node1 = node[5];
    struct tree * node2 = node[2];
    printf("<%c, %c> found parent\n", node1->value, node2->value);

    struct tree * parent = tree_common_parent(tree, node1, node2);
    if (parent == NULL) {
        printf("<%c, %c> not found\n", node1->value, node2->value);
    } else {
        printf("<%c, %c> parent <%c>\n", node1->value, node2->value, parent->value);
    }

    tree_delete(tree);
    exit(EXIT_SUCCESS);
}
