#pragma once

#define ZSKIPLIST_MAXLEVEL 32 /* Should be enough for 2^64 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */

/* ZSETs use a specialized version of Skiplists */
typedef struct zskiplistNode {
    char * ele;
    double score;
    struct zskiplistNode * backward;
    struct zskiplistLevel {
        struct zskiplistNode * forward;
        unsigned long span;
    } level[];
} zskiplistNode;

typedef struct zskiplist {
    struct zskiplistNode * header, * tail;
    unsigned long length;
    int level;
} zskiplist;
