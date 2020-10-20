#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "graph_queue.h"

/*
 * 邻接矩阵图表示的有向图 (Matrix Directed Graph) 
 * 
 * cc -g -O2 -Wall -Wextra -Wno-sign-compare -o mdg mdg.c
 */

struct graph {
    int num;        // 顶点数
    char * vertex;  // 顶点集合
    char matrix[];  // 邻接矩阵 char matrix[num][num] + 顶点集合 char vertex[num]
};

extern inline struct graph * graph_create(int num) {
    struct graph * g = calloc(1, sizeof(struct graph) + num*num + num);
    assert(g && num > 0);
    g->num = num;
    g->vertex = g->matrix + num * num;
    return g;
}

inline void graph_delete(struct graph * g) {
    if (g) free(g);
}

/*
 * 获取邻接矩阵 char matrix[row][col] 
 */
inline char graph_matrix(struct graph * g, int row, int col) {
    assert(row >= 0 && row < g->num && col >= 0 && g->num);
    return g->matrix[row * g->num + col];
}

/*
 * 设置链接矩阵
 * < tail -> head > or (tail - head) 表示边的 尾结点 和 头结点
 */
char * graph_set_position(struct graph * g, char tail, char head) {
    int row = -1, col = -1;

    for (int i = 0; i < g->num; i++) {
        char c = g->vertex[i];
        if (c == tail) {
            row = i;

            if (col >= 0) 
                break;
        }
        if (c == head) {
            col = i;

            if (row >= 0)
                break;
        }
    }

    // 边结点不正确
    if (row == -1 || col == -1) 
        return NULL;

    // 获取边, 设置, 然后返回
    char * edges = g->matrix + row * g->num + col;
    *edges = 1;
    return edges;
}

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
static struct graph * create_example_graph(void) {
    int num = 7;

    struct graph * g = graph_create(num);

    // 设置 顶点集合
    memcpy(g->vertex, "ABCDEFG", num);

    // 通过边关系, 构建 邻接矩阵
    // 在有向图中, 若表示从 顶点 v 到 顶点w 有一条弧. 
    // 其中: v 称为弧尾(tail)或始点(initial node), w 称为弧头(head)或终点(terminal node) 
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

    for (int i = 0; i < (sizeof edges / sizeof *edges); i++) {
        char tail = edges[i][0], head = edges[i][1];
        graph_set_position(g, tail, head);
    }

    return g;
}

/*
 * 返回 顶点(索引)v 的第一个邻接顶点的索引, 失败则返回 -1
 */
static int graph_vertex_first(struct graph * g, int v) {
    if (v >= 0 && v < g->num) {
        for (int i = 0; i < g->num; i++)
            if (graph_matrix(g, v, i))
                return i;
    }
    // 没有找到返回索引 -1
    return -1;
}

/*
 * 返回 顶点(索引)v 相当于 顶点(索引)w 的下一个邻接顶点的索引, 失败则返回 -1
 */
static int graph_vertex_next(struct graph * g, int v, int w) {
    if (v < 0 || v >= g->num || w < 0 || w >= g->num - 1)
        return -1;

    for (int i = w + 1; i < g->num; i++)
        if (graph_matrix(g, v, i))
            return i;

    // 没有找到返回索引 -1
    return -1;
}

/*
 * 深度优先搜索遍历图的递归实现
 */
static void graph_dfs_partial(struct graph * g, int v, bool * visited) {
    visited[v] = true;
    // 业务处理
    printf(" %c", g->vertex[v]);

    // 遍历该顶点的所有邻接顶点. 若是没有访问过, 那么继续往下走
    for (int w = graph_vertex_first(g, v); w >= 0; w = graph_vertex_next(g, v, w)) {
        if (!visited[w]) {
            graph_dfs_partial(g, w, visited);
        }
    }
}

void graph_dfs(struct graph * g) {
    if (!g || g->num <= 0) return;

    // 顶点访问情况记录数组
    bool * visited = calloc(g->num, sizeof(bool));
    assert(visited && g->num > 0);

    // 业务处理
    printf("Matrix Directed Graph DFS:");

    for (int v = 0; v < g->num; v++) {
        if (!visited[v]) {
            graph_dfs_partial(g, v, visited);
        }
    }

    // 业务处理
    putchar('\n');

    free(visited);
}

void graph_bfs(struct graph * g) {
    if (!g || g->num <= 0) return;

    struct graph_queue * q = graph_queue_create(g->num);

    // 业务处理
    printf("Matrix Directed Graph BFS:");

    for (int i = 0; i < g->num; i++) {
        graph_queue_push_visited(q, i);

        // 队列不空继续
        int v;
        while ((v = graph_queue_pop(q)) != -1) {
            // 业务处理
            printf(" %c", g->vertex[v]);

            // 遍历该顶点的所有邻接顶点. 若是没有访问过那就入队, 并处理
            for (int w = graph_vertex_first(g, v); w >= 0; w = graph_vertex_next(g, v, w)) {
                graph_queue_push_visited(q, w);
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
    printf("Matrix Directed Graph:\n");
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
    printf("Matrix Directed Graph vertex:");
    for (int i = 0; i < g->num; i++) {
        printf(" %c", g->vertex[i]);
    }
    putchar('\n');

    printf("Matrix Directed Graph matrix:\n");
    for (int i = 0; i < g->num; i++) {
        for (int j = 0; j < g->num; j++) {
            printf(" %d", graph_matrix(g, i, j));
        }
        putchar('\n');
    }

    // 深度优先搜索遍历图
    graph_dfs(g);

    // 广度优先搜索遍历图
    graph_bfs(g);

    graph_delete(g);

    exit(EXIT_SUCCESS);
}