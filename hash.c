/*
 * Hash function to cfg
 * This code is based on chapter 6 of The C Programming Language
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct nlist {
	struct nlist * next;
	char * key;
	char * value;
};

#define HASHSIZE 101
static struct nlist *hashtab[HASHSIZE]; /* pointer table */

/* form hash value for string s */
unsigned hash(const char *s)
{
	unsigned hashval;
	for(hashval=0; *s!='\0';s++)
		hashval = *s + 31 * hashval;
	return hashval % HASHSIZE;
}

/* look for s in hashtab */
struct nlist *lookup(const char *s)
{
	struct nlist *np;
	for (np = hashtab[hash(s)]; np != NULL; np = np->next)
		if (strcmp(s, np->key) == 0)
			return np; /* found */
	return NULL; /* not found */
}

/* put (key, value) in hashtab */
struct nlist *install(const char *key, const char *value)
{
	struct nlist *np;
	unsigned hashval;
	if ((np = lookup(key)) == NULL) { /* not found */
		np = (struct nlist *) malloc(sizeof(*np));
		if (np == NULL || (np->key = strdup(key)) == NULL)
			return NULL;
		hashval = hash(key);
		np->next = hashtab[hashval];
		hashtab[hashval] = np;
	} else /* already there */
		free((void *) np->value); /*free previous value */
	if ((np->value = strdup(value)) == NULL)
		return NULL;
	return np;
}
