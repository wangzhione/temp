#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 236. 二叉树最近公共祖先

   给定一个二叉树, 找到该树中两个指定结点的最近公共祖先.

   百度百科中最近公共祖先定义: "对于有根树 T 的两个结点 p, q, 
   最近公共祖先表示为一个结点 x, 满足 x 是 p, q 的祖先且 x 的深度尽可能大(一个结点也可以是他自己祖先)"
 */

struct TreeNode {
    int val;
    struct TreeNode * left;
    struct TreeNode * right;
};

struct TreeNode * lowestCommonAncestor(struct TreeNode * root, struct TreeNode * p, struct TreeNode * q) {
    if (root == NULL || root == p || root == q) {
        return root;
    }

    struct TreeNode * left = lowestCommonAncestor(root->left, p, q);
    struct TreeNode * right = lowestCommonAncestor(root->right, p, q);

    // p 和 q 在 root 两边
    if (left != NULL && right != NULL) {
        return root;
    }
    // 要么都在 left, 要么都在 right, 要么没有公共结点
    return left != NULL ? left : right;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 008_236_tree_parent 008_236_tree_parent.c
//
int main(void) {

    exit(EXIT_SUCCESS);
}
