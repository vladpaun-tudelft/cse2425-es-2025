#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef void *(*ResolveCollisionCallback)(void *old_data, void *new_data);
typedef void (*DestroyDataCallback)(void *data);

typedef struct HashNode {
    char *key;
    void *data;
    struct HashNode *next;
} HashNode;

typedef struct HashMap {
    size_t key_space;
    HashNode **buckets;
    unsigned int (*hash)(char *key);
} HashMap;

unsigned int hash(char *key) {
    unsigned int sum = 0;
    while (*key != '\0') {
        sum += (unsigned char)*key;
        key++;
    }
    return sum;
}

size_t get_index(HashMap *hm, char *key) {
    
    unsigned int h = hm->hash(key);
    return (size_t)(h % hm->key_space);
}

HashMap *create_hashmap(size_t key_space) {
    HashMap *hm = malloc(sizeof(HashMap));
    if (!hm) return NULL;

    hm -> key_space = key_space;
    hm-> buckets = calloc(key_space, sizeof(HashNode*));
    if (!hm->buckets) {
        free(hm);
        return NULL;
    }
    hm->hash = hash;

    return hm;
}

void insert_data(HashMap *hm, char *key, void *data, ResolveCollisionCallback resolve_collision) {
    if (!hm || !key) return;

    size_t idx = get_index(hm, key);
    HashNode *node = hm->buckets[idx];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            if (resolve_collision != NULL) {
                node->data = resolve_collision(node->data, data);
            } else {
                node->data = data;
            }
            return;
        }

        node = node->next;
    }

    HashNode *new_node = malloc(sizeof(HashNode));
    if (!new_node) return;

    char *key_copy = malloc(strlen(key) + 1);
    if (!key_copy) {
        free(new_node);
        return;
    }
    strcpy(key_copy,key);

    new_node->key = key_copy;
    new_node->data = data;
    new_node->next = hm->buckets[idx];
    hm->buckets[idx] = new_node;
}

void *get_data(HashMap *hm, char *key) {
    if (!hm || !key) return NULL;

    size_t idx = get_index(hm,key);
    HashNode *node = hm->buckets[idx];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->data;
        }
        node = node->next;
    }

    return NULL;
}

void iterate(HashMap *hm, void (*callback)(char *key, void *data)) {
    if (!hm || !callback) return;

    size_t idx = 0;
    while (idx < hm->key_space) {
        HashNode *node = hm->buckets[idx];
        while (node != NULL) {
            callback(node->key, node->data);
            node = node->next;
        }
        idx++;
    }
}


void remove_data(HashMap *hm, char *key, DestroyDataCallback destroy_data){
    if (!hm || !key) return;

    size_t idx = get_index(hm, key);
    HashNode *node = hm->buckets[idx];
    HashNode *prev = NULL;

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            if (prev == NULL) {
                hm->buckets[idx] = node->next;
            } else {
                prev->next = node->next;
            }

            if (destroy_data != NULL) {
              destroy_data(node->data);
            }

            free(node->key);
            free(node);
            return;
        }

        prev = node;
        node = node->next;
    }
}

void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data) {
    if (!hm) return;

    size_t idx = 0;
    while (idx < hm->key_space) {
        HashNode *node = hm->buckets[idx];
        HashNode *next = NULL;
        while (node != NULL) {
            next = node->next;

            if (destroy_data != NULL) {
                destroy_data(node->data);
            }

            free(node->key);
            free(node);

            node = next;
        }
        idx++;
    }

    free(hm->buckets);
    free(hm);
}

void set_hash_function(HashMap *hm, unsigned int (*hash_function)(char *key)) {
    if (!hm || !hash_function) return;


    size_t key_space = hm->key_space;
    HashNode **old_buckets = hm->buckets;

    HashNode **new_buckets = calloc(key_space,sizeof(HashNode *));
    if (!new_buckets) return;

    for (size_t i = 0; i < key_space; i++) {
        HashNode *node = old_buckets[i];

        while (node != NULL) {
            HashNode *next = node->next;

            unsigned int h = hash_function(node->key);
            size_t idx = (size_t) h % key_space;

            node->next = new_buckets[idx];
            new_buckets[idx] = node;

            node = next;
        }
    }

    hm->hash = hash_function;
    hm->buckets = new_buckets;

    free(old_buckets);
}



void print_stuff(char *key, void *data) {
    int *count = (int *)data;
    printf("%s: %d \n", key, *count);
}

void destroy_int_pointer(void *data) {
    free(data);
}

void count_words(FILE * stream) {
    if (!stream) return;

    HashMap *hm = create_hashmap(1001);
    if (!hm) return;

    char buf[32001];

    fscanf(stream, "%*[^0-9a-zA-Z]\n");
    while (fscanf(stream, "%32000[0-9a-zA-Z]%*[^0-9a-zA-Z]", buf) == 1) {
      int *count = (int *)get_data(hm, buf);

      if (count == NULL) {
        count = malloc(sizeof(int));
        if (!count)
          continue;

        *count = 1;

        insert_data(hm, buf, count, NULL);
      } else {
        (*count)++;
      }
    }

    iterate(hm, print_stuff);

    delete_hashmap(hm, destroy_int_pointer);
}