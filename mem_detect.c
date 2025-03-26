/*
    @author yuanukim
*/
#include "mem_detect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SET_BUCKET_SIZE  101

typedef struct HashNode {
    int line;
    struct HashNode* next;
} HashNode;

typedef struct HashSet {
    HashNode* bucket[HASH_SET_BUCKET_SIZE];
} HashSet;

static MemList memList;

HashSet* hash_set_create(void) {
    HashSet* hs = (HashSet*)malloc(sizeof(HashSet));
    
    for (size_t i = 0; i < HASH_SET_BUCKET_SIZE; ++i) {
        hs->bucket[i] = NULL;
    }
    
    return hs;
}

void hash_set_destroy(HashSet* hs) {
    if (hs) {
        HashNode* cursor;
        HashNode* temp;
        
        for (size_t i = 0; i < HASH_SET_BUCKET_SIZE; ++i) {
            cursor = hs->bucket[i];
            
            while (cursor) {
                temp = cursor->next;
                free(cursor);
                cursor = temp;
            }
        }
        
        free(hs);
    }
}

int hash(int key) {
    return key % HASH_SET_BUCKET_SIZE;
}

HashNode* hash_set_find(HashSet* hs, int key) {
    int hashval = hash(key);
    HashNode* node = hs->bucket[hashval];
    
    while (node) {
        if (node->line == key) {
            return node;
        }
        else {
            node = node->next;
        }
    }
    
    return NULL;
}

void hash_set_add(HashSet* hs, int key) {
    HashNode* node = hash_set_find(hs, key);
    
    if (node != NULL) {
        return;
    }
    
    node = (HashNode*)malloc(sizeof(HashNode));
    node->line = key;
    node->next = NULL;
    
    int hashval = hash(key);
    HashNode* cursor = hs->bucket[hashval];
    
    if (cursor == NULL) {
        hs->bucket[hashval] = node;
    }
    else {
        cursor->next = node;
        cursor = cursor->next;
    }
}

void mem_detect_init(void) {
    memList.head = (MemNode*)malloc(sizeof(MemNode));
    memList.tail = (MemNode*)malloc(sizeof(MemNode));
    
    memList.head->prev = NULL;
    memList.head->next = memList.tail;
    memList.tail->prev = memList.head;
    memList.tail->next = NULL;
}

void mem_detect_destroy(void) {
    MemNode* cursor = memList.head->next;
    MemNode* temp;
    
    while (cursor != memList.tail) {
        temp = cursor->next;
        free(cursor);
        cursor = temp;
    }
    
    free(memList.head);
    free(memList.tail);
}

void* mem_detect_malloc(size_t size, int line, const char* fileName, const char* functionName) {
    MemNode* node = (MemNode*)malloc(sizeof(MemNode) + size);
    MemNode* tailPrev = memList.tail->prev;
    
    node->size = size;
    node->line = line;
    node->fileName = fileName;
    node->functionName = functionName;
    
    node->prev = tailPrev;
    node->next = memList.tail;
    
    tailPrev->next = node;
    memList.tail->prev = node;
    
    return (void*)(node + 1);
}

void* mem_detect_calloc(size_t nmemb, size_t size, int line, const char* fileName, const char* functionName) {
    size_t totalSize = nmemb * size;
    void* memory = mem_detect_malloc(totalSize, line, fileName, functionName);
    memset(memory, 0, totalSize);
    
    return memory;
}

void mem_detect_free(void* mem) {
    MemNode* node = ((MemNode*)mem) - 1;
    MemNode* prev = node->prev;
    MemNode* next = node->next;
    
    prev->next = next;
    next->prev = prev;
    free(node);
}

void mem_detect_print_leak(void) {
    size_t totalLeak = 0;
    HashSet* hs = hash_set_create();
    MemNode* cursor = memList.head->next;
    
    printf("\nmemory leak trace finish.\n");
    
    while (cursor != memList.tail) {
        if (hash_set_find(hs, cursor->line) == NULL) {
            hash_set_add(hs, cursor->line);
            printf("leak found: [FILE]: %s, [LINE]: %d, [FUNC_NAME]: %s.\n", cursor->fileName, cursor->line, cursor->functionName);
        }
        
        totalLeak += cursor->size;
        cursor = cursor->next;
    }
    
    hash_set_destroy(hs);
    
    if (totalLeak > 0) {
        printf("total leak %d bytes.\n", totalLeak);
    }
    else {
        printf("no leaks.\n");
    }
}
