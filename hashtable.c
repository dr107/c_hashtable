#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"
#include "prime.h"

#define HIGH .7 //maximum acceptable load factor
#define TARGET .5 //target load factor when rehashing

struct hashtable {
	void **keys;
	void **values;
	int *occ;
	unsigned long length;
	unsigned long size;

	int (*equals)(void*, void*);
	unsigned long (*hash)(void*);
	
};

static int physeql(void* x, void* y);
static unsigned long djb2(void* x);

hashtable ht_create(unsigned init_size,
		    int (*equals)(void*,void*),
		    unsigned long (*hash)(void*))
{
	if (init_size == 0) {
		init_size = 17;
	}
	hashtable h=malloc(sizeof(struct hashtable));
	h->keys=malloc(init_size*sizeof(void*));
	h->values=malloc(init_size*sizeof(void*));
	h->occ=calloc(init_size, sizeof(int));
	h->length=0;
	h->size=init_size;

	if (equals == NULL) {
		h -> equals = &physeql;
	} else {
		h -> equals = equals;
	}

	if (hash == NULL) {
		h -> hash = &djb2;
	} else {
		h -> hash = hash;
	}

	return h;
}

static void print_err(char *str)
{
	#if PRINT_ERR
	fprintf(stderr, str);
	#endif
}

// return the prime that comes after x
int next_prime(int x)
{
	x++;
	while(1) {
		if(is_prime_mr((size_t) x)) {
			return x;
		}
		x++;
	}
}

void ht_free(hashtable h) {
	if (h == NULL) {
		print_err("Tried to free NULL\n");
		return;
	}

	free(h->keys);
	free(h->values);
	free(h->occ);
	free(h);
}

/*
  if h has an acceptable load factor, do nothing. If not,
  find the next prime that brings the load factor below the
  target, and rehash to that size.
 */
void rehash(hashtable h) {

	if (h == NULL) {
		print_err("Tried to rehash NULL\n");
		return;
	}

	//now the real fight begins, therefore try hashing
	float lf=(float)(h->length+1)/(float)h->size;
	if(lf<HIGH) {
		return;
	}

	int newsize=h->size;
	do {
		newsize=next_prime(newsize);
	} while((float)h->length/(float)newsize>TARGET);
		
  
	hashtable new=ht_create(newsize, h->equals, h->hash);

	//iterate over hashtable, insert stuff into the new hashtable
	for(unsigned i=0; i<h->size; i++) {
		if(!h->occ[i]) continue;
		ht_put(new, h->keys[i], h->values[i]);
	}
  
	//gut new hashtable and free the remains
	h->size=newsize;
	free(h->keys);
	h->keys=new->keys;
	free(h->values);
	h->values=new->values;
	free(h->occ);
	h->occ=new->occ;
	free(new);
}

/*
  Find the index where the key lives in the hashtable. If the key does
  live in the hashtable, its non-negative index will be returned. If it
  does not, a negative number will be returned.

  INVARIANT: h must be non-null, and key must be valid
 */
static long long ht_find(hashtable h, void *key)
{
	if (h == NULL) {
		print_err("Invariant broken in ht_find\n");
		return -1;
	}

	int count=0;
	unsigned long hash=h->hash(&key);
	unsigned long ind=hash % h->size;
	while(count < h->size) {
		if(h->occ[ind] && h->equals(h->keys[ind], key)) {
			return ind;
		} else {
			count++;
			ind=(hash+count*count)%h->size;
		}
	}

	return -1;
}

void ht_remove(hashtable h, void *key) {

	if (h == NULL) {
		print_err("Tried to remove element from NULL hashtable\n");
		return;
	}

	long long ind=ht_find(h, key);
	if (ind >= 0) {
		h->occ[ind]=0;
		h->keys[ind]=0;
		h->values[ind]=0;
		h->length--;
	}
}

void *ht_get(hashtable h, void *key) {
	if (h == NULL) {
		print_err("Tried to get element from NULL hashtable\n");
		return NULL;
	}

	long long ind=ht_find(h, key);
	if (ind >= 0) {
		return h->values[ind];
	} else {
		return NULL;
	}
}

//logic borrowed from wikipedia
void ht_put(hashtable h, void *key, void *val) {

	if (h == NULL) {
		print_err("Tried to put element in NULL hashtable\n");
		return;
	}
	
	int count=0;
	unsigned long hash=h->hash(&key);
	unsigned long ind=hash % h->size;
	rehash(h);
	while(count < h->size) {
		if(!(h->occ[ind])) {
			h->keys[ind]=key;
			h->values[ind]=val;
			h->occ[ind]=1;
			h->length++;
			return;
		} else if(h->occ[ind] && h->equals(h->keys[ind], key)) {
			h->values[ind]=val;
			return;
		} else{
			count++;
			ind=(hash+count*count)%h->size;
		}
	}
}

unsigned ht_len(hashtable h)
{
	if (h == NULL) {
		return 0;
	} else {
		return h->length;
	}
}

unsigned ht_size(hashtable h)
{
	if (h == NULL) {
		return 0;
	} else {
		return h->size;
	}
}

unsigned long djb2(void* x) {
	unsigned long hash=5381;
	unsigned char* str=(unsigned char*)x;
	hash=((hash<<5)+hash)+str[0];
	hash=((hash<<5)+hash)+str[1];
	hash=((hash<<5)+hash)+str[2];
	hash=((hash<<5)+hash)+str[3];
	return hash;
}

int physeql(void *x, void *y)
{
	return (x==y);
}

int ht_containskey(hashtable h, void *key)
{
	if(h == NULL) {
		print_err("Tried to test containment in NULL\n");
	}

	return (ht_find(h, key) < 0) ? 0 : 1;

}
