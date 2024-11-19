#include <stdio.h>
#include <stdlib.h>
#include "mm.h"

int main(){
	int* ptr = my_malloc(sizeof(int));
	
	printf("ptr address: %p\n", ptr);
    	printf("*ptr = %d\n", *ptr);
    	//*ptr = 69;
    	//printf("ptr address: %p\n", ptr);
    	//printf("*ptr = %d\n", *ptr);

    	my_free(ptr);


    	int* arr = my_calloc(10,sizeof(int));
	printf("arr address: %p\n", arr);
    	for(int i=0;i<10;i++)
    		arr[i] = 13;
    	for(int i=0;i<10;i++)
    		printf("arr[%d] = %d	arr[%d] address: %p\n",i,*(arr+i),i,(arr+i));
	
	
	int* arr2 = my_realloc(arr, sizeof(int)*20);
	printf("arr2 address: %p\n", arr2);
	for(int i=0;i<20;i++)
    		printf("arr2[%d] = %d	arr2[%d] address: %p\n",i,*(arr2+i),i,(arr2+i));

	//printf("arr address: %p\n", arr);
    	//printf("arr2 address: %p\n", arr2);

    	my_free(arr2);

	return 0;
}

