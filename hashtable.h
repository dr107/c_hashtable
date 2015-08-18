#include <stdio.h>
#include <stdlib.h>

/*
  If this is nonzero, error messages will be printed to stderr when
  invariants are broken, either by your mistake, or (hopefully not),
  mine. 
 */
#define PRINT_ERR 1

typedef struct hashtable* hashtable;

/*
  Allocate a new hashtable. This hashtable will later have to be
  freed using the destructor ht_free. The first parameter is an
  initail capacity for the table. Prime sizes are best, but if
  a zero value is given for this, a default will be used.

  The second and third arguments are used to specify the properties
  of the key set. The equals function defines the equality of keys.
  This function should return 0 if two pointers are not "equal," by
  the desired definition, and some other number if they are. Hash is
  a hashing function, and should have all the properties of a hashing
  funtion. Remember that the equality funtion must be symmetric,
  transitive, and reflexive, and that if equal(x,y) is true, then hash(x)
  must be equal to hash(y).

  If either of these function arguments are NULL, a default function
  is used. The default equality operation used is numerical
  comparison of the pointers, and the default hash is a djb2 hash of
  the numerical value of the pointer.

  INVARIANTS: The two function pointers must be either pointers to
  functions with the above characteristics, or NULL.
 */
hashtable ht_create(unsigned init_size,
		    int (*equals)(void*, void*),
		    unsigned long (*hash)(void*));

/*
  The aforementioned hashtable destructor.

  INVARIANT: h must not be null.
 */
void ht_free(hashtable h);

/*
  Put a key-value pair into the hashtable.

  INVARIANTS: h must not be null, and the key value must be able to be used
  by the table's hash and equality functions.

  If either of the functions given during allocation involves dereferencing of
  the pointers given to them, it is the user's responsibility to ensure the
  pointers stored as keys in the table are valid for the duration of
  the table's lifetime.
 */
void ht_put(hashtable h, void *key, void *val);

/*
  Fetch the value associated with a given key.

  If there is a key in the hashtable that is equal (according to the table's equality operator)
  to the given key, return the value associated with it. If there is not, return NULL.

  Note that NULL may be a legitimate value in the table, so this function is
  inappropriate as a containment test. Use ht_containskey instead.

  INVARIANTS: h must not be null, and the key value must be usable by the table's
  hash and equality functions.

 */
void *ht_get(hashtable h, void *key);


/*
  get the length, i.e. the number of kv pairs stored, of the given hashtable.

  INVARIANT: h must be non-null
 */
unsigned ht_len(hashtable h);

/*
  get the size, or capacity of the hashtable, from the given hashtable.

  INVARIANT: h must not be null
 */
unsigned ht_size(hashtable h);

/*
  Delete the given element from the given hashtable. If key is not in the keyset
  of the hashtable, no operation is performed.

  INVARIANTS: h must be non-null, and key must be usable.
 */
void ht_remove(hashtable h, void *key);

/*
  Evaluate whether the given key is in the key set of the hashtable. If it is not,
  0 is returned. If it is, something else is returned.
 */
int ht_containskey(hashtable h, void *key);
