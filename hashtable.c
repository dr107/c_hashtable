#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"

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
static int is_prime_mr(size_t);

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

/* ---------------------- PRIME NUMBER FUNCTIONS----------------- */
/* These are taken from Rosetta code. I take no responsibility for their style*/
typedef int bool;
#define true 1
#define false 0

//bool is_prime_mr(size_t n);
// calcul a^n%mod
static size_t power(size_t a, size_t n, size_t mod)
{
  size_t power = a;
  size_t result = 1;

  while (n)
    {
      if (n & 1)
	result = (result * power) % mod;
      power = (power * power) % mod;
      n >>= 1;
    }
  return result;
}

// n−1 = 2^s * d with d odd by factoring powers of 2 from n−1
static bool witness(size_t n, size_t s, size_t d, size_t a)
{
  size_t x = power(a, d, n);
  size_t y=0;

  while (s) {
    y = (x * x) % n;
    if (y == 1 && x != 1 && x != n-1)
      return false;
    x = y;
    --s;
  }
  if (y != 1)
    return false;
  return true;
}

/*
 * if n < 1,373,653, it is enough to test a = 2 and 3;
 * if n < 9,080,191, it is enough to test a = 31 and 73;
 * if n < 4,759,123,141, it is enough to test a = 2, 7, and 61;
 * if n < 1,122,004,669,633, it is enough to test a = 2, 13, 23, and 1662803;
 * if n < 2,152,302,898,747, it is enough to test a = 2, 3, 5, 7, and 11;
 * if n < 3,474,749,660,383, it is enough to test a = 2, 3, 5, 7, 11, and 13;
 * if n < 341,550,071,728,321, it is enough to test a = 2, 3, 5, 7, 11, 13, and 17.
 */

static bool is_prime_mr(size_t n) {
  if (((!(n & 1)) && n != 2 ) || (n < 2) || (n % 3 == 0 && n != 3))
    return false;
  if (n <= 3)
    return true;

  size_t d = n / 2;
  size_t s = 1;
  while (!(d & 1)) {
    d /= 2;
    ++s;
  }

  if (n < 1373653)
    return witness(n, s, d, 2) && witness(n, s, d, 3);
  if (n < 9080191)
    return witness(n, s, d, 31) && witness(n, s, d, 73);
  if (n < 4759123141)
    return witness(n, s, d, 2) && witness(n, s, d, 7) && witness(n, s, d, 61);
  if (n < 1122004669633)
    return witness(n, s, d, 2) && witness(n, s, d, 13) && witness(n, s, d, 23) && witness(n, s, d, 1662803);
  if (n < 2152302898747)
    return witness(n, s, d, 2) && witness(n, s, d, 3) && witness(n, s, d, 5) && witness(n, s, d, 7) && witness(n, s, d, 11);
  if (n < 3474749660383)
    return witness(n, s, d, 2) && witness(n, s, d, 3) && witness(n, s, d, 5) && witness(n, s, d, 7) && witness(n, s, d, 11) && witness(n, s, d, 13);
  return witness(n, s, d, 2) && witness(n, s, d, 3) && witness(n, s, d, 5) &&
    witness(n, s, d, 7) && witness(n, s, d, 11) && witness(n, s, d, 13) && witness(n, s, d, 17);
}


