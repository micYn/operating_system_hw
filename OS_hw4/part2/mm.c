#include "mm.h"

static Header base;
static Header *freep = NULL;

Header *morecore(size_t num){
	char *cp=NULL;
	Header *up;
	if(num < MALLOC){
		num = MALLOC;
	}
	cp = (char*)sbrk(num*sizeof(Header));
	if(cp == (char*)-1){
		return NULL;
	}
	up = (Header*)cp;
	up->s.size = num;
	my_free((void *)(up+1));

	return freep;
}
void *my_malloc(size_t size){
	Header *p,*prevp;
	unsigned nunits;
	nunits = (size+sizeof(Header)-1)/sizeof(Header)+1;
	prevp = freep;
	if(prevp == NULL){
		base.s.ptr=&base;
		freep=&base;
		prevp=&base;
		base.s.size=0;
	}
	for(p=prevp->s.ptr ; ; prevp=p,p=p->s.ptr){
		if(p->s.size >= nunits){
			if(p->s.size == nunits){
				prevp->s.ptr=p->s.ptr;
			}
			else{
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
			}
			freep=prevp;
			return(void*)(p+1);
		}
		if(p == freep){
			p = morecore(nunits);
			if(p == NULL)	return NULL;
		}
	}
}

void *my_realloc(void* ptr,size_t size){
	if(!ptr)	return my_malloc(size);
	
	Header *p = (Header*)ptr-1;
	size_t n = (p->s.size-1)*sizeof(Header);
	void *temp;
	
	if(n >= size)	return ptr;
	
	temp = my_malloc(size);
	if(temp){
		memset(temp,0,size);	
		memcpy(temp,ptr,n);
		my_free(ptr);
	}
	return temp;	
}

void *my_calloc(size_t nmemb,size_t size){
	void *ptr=my_malloc(nmemb*size);
	if(ptr){
		memset(ptr,0,nmemb*size);
	}
	return ptr;
}

void my_free(void* ptr){
	Header *p,*bp=(Header*)ptr-1;
	for(p=freep ; !(bp > p && bp < p->s.ptr) ; p=p->s.ptr){
		if( p>=p->s.ptr && (bp>p || bp<p->s.ptr) )	break;
	}
	
	if(bp+bp->s.size == p->s.ptr){
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	}
	else	bp->s.ptr = p->s.ptr;
	
	if(p+p->s.size == bp){
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	}
	else	p->s.ptr = bp;
	
	freep=p;
}
