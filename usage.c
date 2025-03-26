#include "mem_detect.h"

/*
	after include the header file: mem_detect.h 
	then you can define the macros below to make the detector work. do not reverse this order.
*/
#define malloc(size) \
	MEM_DETECT_MALLOC(size)
	
#define calloc(nmemb, size) \
	MEM_DETECT_CALLOC(nmemb, size)

#define free(mem) \
	mem_detect_free(mem)

void func_c(void) {
	char* memory = calloc(999, sizeof(char));
}

void func_b(void) {
	func_c();
}

void func_a(void) {
	func_b();
}

int main() {
	mem_detect_init();
	
	/* here, memory detector begins to work. */
	int* memory = malloc(sizeof(int));
	free(memory);
	
	memory = malloc(10 * sizeof(int));
	func_a();
	
	/* now, we can know where cause the leaks. */
	mem_detect_print_leak();
	mem_detect_destroy();
	return 0;
}
