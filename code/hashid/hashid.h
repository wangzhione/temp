#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct hashid_node {
    int id;
    struct hashid_node * next;
};

struct hashid {
    int hashmod;
    int cap;
    int len;
    struct hashid_node * array;
    struct hashid_node ** hash;
};

static void hashid_init(struct hashid * hi, int max) {
    int hashcap = 16;
    while (hashcap < max) {
        hashcap *= 2;
    }
    hi->hashmod = hashcap - 1;
    hi->cap = max;
    hi->len = 0;
    hi->array = malloc(max * sizeof(struct hashid_node));
    for (int i = 0; i < max; i++) {
        hi->array[i].id = -1;
        hi->array[i].next = NULL;
    }
    hi->hash = calloc(hashcap, sizeof(struct hashid_node *));
}

static void hashid_clear(struct hashid * hi) {
    free(hi->array);
    free(hi->hash);
    hi->array = NULL;
    hi->hash = NULL;
    hi->hashmod = 1;
    hi->cap = 0;
    hi->len = 0;
}

static int hashid_lookup(struct hashid * hi, int id) {
    int h = id & hi->hashmod;
    struct hashid_node * node = hi->hash[h];
    while (node) {
        if (node->id == id)
            return node - hi->array;
        node = node->next;
    }
    return -1;
}

static int hashid_remove(struct hashid * hi, int id) {
    int h = id & hi->hashmod;
    struct hashid_node * node = hi->hash[h];
    if (NULL == node)
        return -1;

    if (node->id == id) {
        hi->hash[h] = node->next;
        goto ret_clr;
    }

    while (node->next) {
        if (node->next->id == id) {
            struct hashid_node * temp = node->next;
            node->next = temp->next;
            node = temp;
            goto ret_clr;
        }

        node = node->next;
    }
    return -1;

ret_clr:
    node->id = -1;
    node->next = NULL;
    --hi->len;
    return node - hi->array;
}

static int hashid_insert(struct hashid * hi, int id) {
    int h;
    struct hashid_node * node = NULL;
    for (h = 0; h < hi->cap; h++) {
        int index = (h + id) % hi->cap;
        if (hi->array[index].id == -1) {
            node = hi->array + index;
            break;
        }
    }
    assert(node && node->next == NULL);
    node->id = id;
    ++hi->len;

    h = id & hi->hashmod;
    node->next = hi->hash[h];
    hi->hash[h] = node;

    return node - hi->array;
}

static inline int hashid_full(struct hashid * hi) {
    return hi->len == hi->cap;
}
