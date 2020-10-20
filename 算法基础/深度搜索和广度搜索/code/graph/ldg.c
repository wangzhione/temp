#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "graph_queue.h"

/*
 * 邻接链表图表示的有向图 (List Directed Graph) 
 * 
 * cc -g -O2 -Wall -Wextra -Wno-sign-compare -o ldg ldg.c
 */

// 邻接链表中表对应链表的顶点
struct graph_node {
    struct graph_node * next;       // 指向下一条弧的指针
    int v;                          // 该边锁指向的顶点的索引位置
};

inline struct graph_node * graph_node_create(int v) {
    struct graph_node * node = malloc(sizeof(struct graph_node));
    node->next = NULL;
    node->v = v;
    return node;
}

void graph_node_delete(struct graph_node * list) {
    while (list) {
        struct graph_node * next = list->next;
        free(list);
        list = next;
    }
}

// 将 node 链接到 list 的末尾
struct graph_node * graph_insert_last(struct graph_node * list, struct graph_node * node) {
    if (!list)
        list = node;
    else {
        struct graph_node * tail = list;
        while (tail->next) {
            tail = tail->next;
        }
        tail->next = node;
    } 
    return list;
}

// 邻接链表中顶点
struct graph_vertex {
    char data;                      // 顶点数据信息
    struct graph_node * edge;       // 指向依附该顶点的弧
};

// vertex 新插入结点
inline void graph_vertex_insert(struct graph_vertex * vertex, struct graph_node * node) {
    vertex->edge = graph_insert_last(vertex->edge, node);
}

// 邻接链表
struct graph {
    int num;                        // 图顶点数量
    struct graph_vertex vertex[];   // 图中顶点链表
};

extern inline struct graph * graph_create(int num) {
    struct graph * g = calloc(1, sizeof(struct graph) + sizeof(struct graph_vertex) * num);
    assert(num > 0 && g);
    g->num = num;
    return g;
}

inline void graph_delete(struct graph * g) {
    if (!g) return;

    for (int i = 0; i < g->num; i++) {
        graph_node_delete(g->vertex[i].edge);
    }

    free(g);
}

// 返回 c 在 graph::vertex 链接链表中的位置
static int graph_get_position(struct graph * g, char c) {
    for (int i = 0; i < g->num; i++) 
        if (g->vertex[i].data == c) 
            return i;
    return -1;
}

/* 
 * 构建邻接链表图表示的有向图
 * 
 * | <=> ↑  +  ↓
 * - <=> ← + →
 * 
 *      A
 *      ↓
 * C ← B → F
 * ↑ ↘ |    ↓
 * D ← E    G
 * 
 */
static struct graph * create_example_graph(void) {
    // 构建顶点
    char vertex[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G' };

    struct graph * g = graph_create(LEN(vertex));

    for (int i = 0; i < LEN(vertex); i++) {
        g->vertex[i].data = vertex[i];
    }

    // 通过边关系, 构建 邻接链表
    char edges[][2] = {
        {'A', 'B'}, 
        {'B', 'C'}, 
        {'B', 'E'}, 
        {'B', 'F'}, 
        {'C', 'E'}, 
        {'D', 'C'}, 
        {'E', 'B'}, 
        {'E', 'D'}, 
        {'F', 'G'}
    };

    for (int i = 0; i < LEN(edges); i++) {
        char tail = edges[i][0], head = edges[i][1];

        int v = graph_get_position(g, tail);
        int w = graph_get_position(g, head);

        // 跳过无效的边
        if (v == -1 || w == -1) {
            continue;
        }

        struct graph_node * nodew = graph_node_create(w);

        graph_vertex_insert(g->vertex + v, nodew);
    }

    return g;
}

// 深度优先搜索遍历图的递归实现
static void graph_dfs_partial(struct graph * g, int v, bool * visited) {
    visited[v] = true;
    // 业务处理
    printf(" %c", g->vertex[v].data);

    // 遍历该顶点的所有邻接顶点. 若是没有访问过, 那么继续往下走
    struct graph_node * list = g->vertex[v].edge;
    while (list) {
        if (!visited[list->v])
            graph_dfs_partial(g, list->v, visited);
        list = list->next;
    }
}

// 深度优先搜索遍历图
void graph_dfs(struct graph * g) {
    if (!g || g->num <= 0) return;

    // 顶点访问情况记录数组
    bool * visited = calloc(g->num, sizeof(bool));
    assert(visited && g->num > 0);

        // 业务处理
    printf("List Directed Graph DFS:");

    for (int v = 0; v < g->num; v++) {
        if (!visited[v]) {
            graph_dfs_partial(g, v, visited);
        }
    }

    // 业务处理
    putchar('\n');

    free(visited);
}

// 广度优先搜索(类比树的层次遍历)
void graph_bfs(struct graph * g) {
    if (!g || g->num <= 0) return;

    struct graph_queue * q = graph_queue_create(g->num);

    // 业务处理
    printf("List Directed Graph BFS:");

    for (int i = 0; i < g->num; i++) {
        graph_queue_push_visited(q, i);

        // 队列不空继续
        int v;
        while ((v = graph_queue_pop(q)) != -1) {
            // 业务处理
            printf(" %c", g->vertex[v].data);

            // 遍历该顶点的所有邻接顶点. 若是没有访问过那就入队, 并处理
            struct graph_node * list = g->vertex[v].edge;
            while (list) {
                graph_queue_push_visited(q, list->v);
                list = list->next;
            }
        }
    }

    // 业务处理
    putchar('\n');

    graph_queue_delete(q);
}

int main(void) {
    struct graph * g = create_example_graph();

    // 输出图
    printf("List Directed Graph:\n");
    /* 
    * 构建邻接矩阵有向图
    * 
    * | <=> ↑  +  ↓
    * - <=> ← + →
    * 
    *      A
    *      ↓
    * C ← B → F
    * ↑ ↘ |    ↓
    * D ← E    G
    * 
    */
    printf("     A\n");
    printf("     ↓\n");
    printf(" C ← B → F\n");
    printf(" ↑ ↘ |   ↓\n");
    printf(" D ← E   G\n");

    // 输出边和邻接矩阵关系
    printf("List Directed Graph vertex:");
    for (int i = 0; i < g->num; i++) {
        printf(" %c", g->vertex[i].data);
    }
    putchar('\n');

    printf("List Directed Graph edges:\n");
    for (int i = 0; i < g->num; i++) {
        printf("%c | ", g->vertex[i].data);

        struct graph_node * list = g->vertex[i].edge;
        while (list) {
            printf("%c -> ", g->vertex[list->v].data);
            list = list->next;
        }

        printf("NULL\n");
    }

    // 深度优先搜索遍历图
    graph_dfs(g);

    // 广度优先搜索遍历图
    graph_bfs(g);

    graph_delete(g);
    exit(EXIT_SUCCESS);
}