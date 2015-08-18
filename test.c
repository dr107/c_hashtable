#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hashtable.h"

int eq(void *x, void *y)
{
	return x==y;
}

unsigned long hash(void *vx) {
	unsigned long x = *((unsigned*) vx);
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x) * 0x45d9f3b;
	x = ((x >> 16) ^ x);
	return x;
}

//from stackoverflow
int random_in_range (unsigned int min, unsigned int max) {
	int base_random = rand(); /* in [0, RAND_MAX] */
	if (RAND_MAX == base_random) return random_in_range(min, max);
	/* now guaranteed to be in [0, RAND_MAX) */
	int range       = max - min,
		remainder   = RAND_MAX % range,
		bucket      = RAND_MAX / range;
	/* There are range buckets, plus one smaller interval
	   within remainder of RAND_MAX */
	if (base_random < RAND_MAX - remainder) {
		return min + base_random/bucket;
	} else {
		return random_in_range (min, max);
	}
}

void test_rm(hashtable h)
{

	for(long i=0; i<10; i++) {
		ht_put(h, (void*) i, (void*) i);
	}

	ht_remove(h, (void*) 0);
	// shouldn't screw anything up
	ht_remove(h, (void*) 0);
	ht_remove(h, (void*) 1);
	ht_remove(h, (void*) 2);

	for(long i=0; i<10; i++) {
		int contained = ht_containskey(h, (void*)i);
		int correct = ((i<=2) && !contained) || ((i>2) && contained);
		if(!correct) {
			printf("Failed to properly delete an element\n");
			return;
		}
	}
	
	printf("Delete test passed\n");
}

void test_add_contains_get(hashtable h, unsigned n)
{
	long *randos = malloc(n*sizeof(long));
 	for(long i=0; i<n; i++) {
		long r= (long) random_in_range(0, 100);
		randos[i] = r;
		ht_put(h, (void*)i, (void*)r);
	}
	for(long i=0; i<n; i++) {
		int cont = ht_containskey(h, (void*)i); 
		long got = (long)ht_get(h, (void*)i);
		if(!cont || (randos[i] != got)) {
			printf("Failed to get key %ld\n", i);
			return;
		}
	} 
	free(randos);
	printf("Add/contains/get test passed\n");
}

int main() {
	hashtable h;
	srand(time(NULL));

	h = ht_create(5, NULL, NULL);
	test_add_contains_get(h, 10000);
	ht_free(h);

	h = ht_create(5, NULL, NULL);
	test_rm(h);
	ht_free(h);

	// test that using custom operators doesn't cause
	// problems
	h = ht_create(5, &eq, &hash);
	test_add_contains_get(h, 10000);
	ht_free(h);

	h = ht_create(5, &eq, &hash);
	test_rm(h);
	ht_free(h);

	return 0;
}
