#ifndef MM_H
#define MM_H

#include<stddef.h>
#include<unistd.h>
#include<string.h>
#define MALLOC 1024
union header{
	struct{
		union header *ptr;
		unsigned size;
	}s;
	long x;
};
typedef union header Header;
Header *morecore(size_t num);
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);
void *my_calloc(size_t nmemb, size_t size);

#endif // MM_H
