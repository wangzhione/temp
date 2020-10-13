#include "q.h"
#include "stack.h"

extern struct tree * tree_create(void);
extern void tree_delete(struct tree * root);
extern void tree_preorder(struct tree * root);
extern void tree_inorder(struct tree * root);
extern void tree_postorder(struct tree * root);
extern void tree_level(struct tree * root);

int main(void) {
    printf("Hello, 世界!\n");

    struct tree * root = tree_create();

    // 二叉树 - 深度优先搜索 - 前序遍历
    tree_preorder(root);
    putchar('\n');

    // 二叉树 - 深度优先搜索 - 中序遍历
    tree_inorder(root);
    putchar('\n');

    // 二叉树 - 深度优先搜索 - 后续遍历
    tree_postorder(root);
    putchar('\n');

    // 二叉树 - 深度广度搜索 - 层次遍历
    tree_level(root);
    putchar('\n');

    tree_delete(root);

    exit(EXIT_SUCCESS);
}

/**
 * 方便测试和描述算法, 用 int node 构建 
 */
typedef int node_t;

static void node_printf(node_t value) {
    printf(" %2d", value);
}

/****************************** 二叉树 ******************************/
struct tree {
    struct tree * left;
    struct tree * right;

    node_t node;
};

static inline struct tree * tree_new(node_t value) {
    struct tree * node = malloc(sizeof (struct tree));
    node->left = node->right = NULL;
    node->node = value;
    return node;
}

/*
 * 构建简单用于测试展示的二叉树
 * 
 *                  1
 *                 / \
 *                2   3
 *                /   \
 *               4     5
 *              /       \
 *             6         7
 *            / \       / \
 *           8   9    10   11
 *                \   /     \
 *                12 13     14
 * 
 * 前序遍历: 1 -> 2 -> 4 -> 6 -> 8 -> 9 -> 12 -> 3 -> 5 -> 7 -> 10 -> 13 -> 11 -> 14
 * 中序遍历: 8 -> 6 -> 9 -> 12 -> 4 -> 2 -> 1 -> 3 -> 5 -> 13 -> 10 -> 7 -> 11 -> 14
 * 后续遍历: 8 -> 12 -> 9 -> 6 -> 4 -> 2 -> 13 -> 10 -> 14 -> 11 -> 7 -> 5 -> 3 -> 1
 */
struct tree * 
tree_create(void) {
    // 构建所有二叉树结点
    struct tree * node1 = tree_new(1);
    struct tree * node2 = tree_new(2);
    struct tree * node3 = tree_new(3);
    struct tree * node4 = tree_new(4);
    struct tree * node5 = tree_new(5);
    struct tree * node6 = tree_new(6);
    struct tree * node7 = tree_new(7);
    struct tree * node8 = tree_new(8);
    struct tree * node9 = tree_new(9);
    struct tree * node10 = tree_new(10);
    struct tree * node11 = tree_new(11);
    struct tree * node12 = tree_new(12);
    struct tree * node13 = tree_new(13);
    struct tree * node14 = tree_new(14);

    // 按层构建数结构
    node1->left = node2;
    node1->right = node3;

    node2->left = node4;
    node3->right = node5;

    node4->left = node6;
    node5->right = node7;

    node6->left = node8;
    node6->right = node9;
    node7->left = node10;
    node7->right = node11;

    node9->right = node12;
    node10->left = node13;
    node11->right = node14;

    return node1;
}

static void tree_delete_(struct tree * node) {
    if (node->left) tree_delete_(node->left);
    if (node->right) tree_delete_(node->right);
    free(node);
}

void 
tree_delete(struct tree * root) {
    if (root) tree_delete_(root);
}

/****************************** 深度遍历 ******************************/

static void tree_preorder_(struct stack * s) {
    struct tree * top;

    // 2.1 访问栈顶结点, 并将其出栈
    while ((top = stack_pop_top(s))) {
        // 2.2 做业务处理
        node_printf(top->node);

        // 3. 如果根结点存在右孩子, 则将右孩子入栈
        if (top->right)
            stack_push(s, top->right);
        // 4. 如果根结点存在左孩子, 则将左孩子入栈
        if (top->left)
            stack_push(s, top->left);
    }
}

/*
 * 前序遍历:
 *      根结点 -> 左子树 -> 右子树
 * 
 * 遍历算法:
 *      1. 先将根结点入栈
        2. 访问栈顶结点, 做业务处理, 并将其出栈
        3. 如果根结点存在右孩子, 则将右孩子入栈
        4. 如果根结点存在左孩子, 则将左孩子入栈
        重复 2 - 4 直到栈空
 */
void 
tree_preorder(struct tree * root) {
    if (!root) return;

    struct stack s[1]; 
    stack_init(s);

    // 1. 先将根结点入栈
    stack_push(s, root);
    // 重复 2 - 4 直到栈空
    tree_preorder_(s);

    stack_free(s);
}

static void tree_inorder_(struct tree * node, struct stack * s) {
    // 4. 重复第 2 - 3步, 直到栈空并且不存在待访问结点
    while (!stack_empty(s) || node) {
        // 2. 将当前结点的所有左孩子入栈, 直到左孩子为空
        while (node) {
            // 1. 先将根结点入栈
            stack_push(s, node);
            node = node->left;
        }

        // 3.1 弹出并访问栈顶元素,
        node = stack_pop_top(s);
        // 3.2 做业务处理.
        node_printf(node->node);
        // 4.1 如果栈顶元素存在右孩子
        node = node->right;
    }
}

/*
 * 中序遍历:
 *      左子树 -> 根结点 -> 右子树
 * 
 * 遍历算法:
 *      1. 先将根结点入栈
        2. 将当前结点的所有左孩子入栈, 直到左孩子为空
        3. 弹出并访问栈顶元素, 做业务处理.
        4. 如果栈顶元素存在右孩子, 重复第 2 - 3步, 直到栈空并且不存在待访问结点
 */
void
tree_inorder(struct tree * root) {
    if (!root) return;

    struct stack s[1]; 
    stack_init(s);

    tree_inorder_(root, s);

    stack_free(s);
}

static void tree_postorder_(struct stack * s) { 
    struct tree * top;
    // 记录前一次出栈结点
    struct tree * last = NULL;

    // 重复 1-2 直到栈空
    while ((top = stack_top(s))) {
        // 2.2 如果我们发现它左右子结点都为空, 则可以做业务处理;
        // 2.3 或者，如果发现前一个出栈的结点是它的左结点或者右子结点，则可以做业务处理;
        if ((!top->left && !top->right) || (last && (last == top->left || last == top->right))) {
            // 做业务处理
            node_printf(top->node);
            // 出栈
            stack_pop(s);
            // 记录此次出栈结点
            last = top;
         } else {
            // 2.4. 否则，表示这个结点是一个新的结点，需要尝试将其右左子结点依次入栈.
            if (top->right)
                stack_push(s, top->right);
            if (top->left)
                stack_push(s, top->left);
         }
    }
}

/*
 * 后序遍历:
 *      左子树 -> 右子树 -> 根结点
 * 
 * 遍历算法:
 *      1. 拿到一个结点, 先将其入栈, 则它一定在比较靠栈底的地方, 比较晚出栈
        2. 在出栈时候判断到底是只有左结点还是只有右结点或者是两者都有或者是都没有.
           如果我们发现它左右子结点都为空, 则可以做业务处理;
           或者，如果发现前一个出栈的结点是它的左结点或者右子结点，则可以做业务处理;
           否则，表示这个结点是一个新的结点，需要尝试将其右左子结点依次入栈.
        重复 1-2 直到栈空
 */
void 
tree_postorder(struct tree * root) {
    if (!root) return;

    struct stack s[1]; 
    stack_init(s);

    // 1. 先将根结点入栈
    stack_push(s, root);
    // 重复 1 - 4 直到栈空
    tree_postorder_(s);

    stack_free(s);
}

/****************************** 深度遍历 ******************************/

/****************************** 广度遍历 ******************************/

/*
 * 层次遍历:
 *      根结点 -> 下一层 | 左结点 -> 右结点
 *
 * 遍历算法:
 *      1. 对于不为空的结点, 先把该结点加入到队列中
 *      2. 从队中弹出结点, 并做业务处理, 尝试将左结点右结点依次压入队列中
 *      重复 1 - 2 指导队列为空 
 */
void
tree_level(struct tree * root) {
    if (!root) return;

    q_t q;
    q_init(q);

    // 1. 对于不为空的结点, 先把该结点加入到队列中
    q_push(q, root);
    do {
        // 2.1 从队中弹出结点, 并做业务处理
        struct tree * node = q_pop(q);

        node_printf(node->node);

        // 2.2 尝试将左结点右结点依次压入队列中
        if (node->left) 
            q_push(q, node->left);
        if (node->right)
            q_push(q, node->right);

    // 重复 1 - 2 指导队列为空
    } while (q_exist(q));

    q_free(q);
}

/****************************** 广度遍历 ******************************/

/****************************** 二叉树 ******************************/
