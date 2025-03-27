/*
    @author  yuanukim
    @brief   a detector to detect if any memory created by malloc() is not freed, written in C99.
    @note    this detector does not support realloc().
			 this detector assumes that the working environment has enough memory, so the underlying malloc(), calloc() call are not checked.
*/
#ifndef __MEM_DETECT_H__
#define __MEM_DETECT_H__

#include <stddef.h>

/*
    initialize the memory detector.
*/
void mem_detect_init(void);

/*
    destroy the memory detector.
*/
void mem_detect_destroy(void);

/*
    same usage as malloc(), but with extra parameters. you can just use MEM_DETECT_MALLOC() macro instead.
    you can only call mem_detect_free() on the returned value to free them.
*/
void* mem_detect_malloc(size_t size, int line, const char* fileName, const char* functionName);

/*
	same usage as calloc().
	you can only call mem_detect_free() on the returned value to free them.
*/
void* mem_detect_calloc(size_t nmemb, size_t size, int line, const char* fileName, const char* functionName);

/*
    same usage as free().
*/
void mem_detect_free(void* mem);

/*
    print all the memory information which were not be freed before.
*/
void mem_detect_print_leak(void);

/*
    a handy macro, simplify the usage of the mem_detect_malloc() function.
*/
#define MEM_DETECT_MALLOC(size) \
    mem_detect_malloc((size), __LINE__, __FILE__, __func__)

/*
    a handy macro, simplify the usage of the mem_detect_calloc() function.
*/
#define MEM_DETECT_CALLOC(nmemb, size) \
    mem_detect_calloc((nmemb), (size), __LINE__, __FILE__, __func__)

#endif
