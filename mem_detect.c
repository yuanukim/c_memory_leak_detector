/*
	@author yuanukim
*/
#include "mem_detect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static MemList memList;

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
    MemNode* cursor = memList.head->next;
    
    while (cursor != memList.tail) {
        printf("[LINE]: %d, [FILE]: %s, [FUNC_NAME]: %s, leak %d bytes.\n", cursor->line, cursor->fileName, cursor->functionName, cursor->size);
        cursor = cursor->next;
    }
}
