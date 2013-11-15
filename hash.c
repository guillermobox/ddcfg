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

/* hash: form hash value for string s */
unsigned hash(char *s)
{
	unsigned hashval;
	for(hashval=0; *s!='\0';s++)
		hashval = *s + 31 * hashval;
	return hashval % HASHSIZE;
}

/* lookup: look for s in hashtab */
struct nlist *lookup(char *s)
{
	struct nlist *np;
	for (np = hashtab[hash(s)]; np != NULL; np = np->next)
		if (strcmp(s, np->key) == 0)
			return np; /* found */
	return NULL; /* not found */
}

/* install: put (name, defn) in hashtab */
struct nlist *install(char *name, char *defn)
{
	struct nlist *np;
	unsigned hashval;
	if ((np = lookup(name)) == NULL) { /* not found */
		np = (struct nlist *) malloc(sizeof(*np));
		if (np == NULL || (np->key = strdup(name)) == NULL)
			return NULL;
		hashval = hash(name);
		np->next = hashtab[hashval];
		hashtab[hashval] = np;
	} else /* already there */
		free((void *) np->value); /*free previous defn */
	if ((np->value = strdup(defn)) == NULL)
		return NULL;
	return np;
}
