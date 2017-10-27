/*
 * Hash function to cfg
 * This code is based on chapter 6 of The C Programming Language
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash.h"

static struct nlist *hashtab[HASHSIZE];	/* pointer table */

/* form hash value for string s */
__DDCFG_EXPORT__ unsigned hash(const char *s)
{
	unsigned hashval;
	for (hashval = 0; *s != '\0'; s++)
		hashval = *s + 31 * hashval;
	return hashval % HASHSIZE;
}

/* look for s in hashtab */
struct nlist *lookup(const char *s)
{
	struct nlist *np;
	for (np = hashtab[hash(s)]; np != NULL; np = np->next)
		if (strcmp(s, np->key) == 0)
			return np;	/* found */
	return NULL;		/* not found */
}

/* put (key, value) in hashtab */
__DDCFG_EXPORT__ struct nlist *install(const char *key, const char *value)
{
	struct nlist *np;
	unsigned hashval;
	if ((np = lookup(key)) == NULL) {	/* not found */
		np = (struct nlist *) calloc(1, sizeof(*np));
		if (np == NULL || (np->key = strdup(key)) == NULL)
			return NULL;
		hashval = hash(key);
		np->next = hashtab[hashval];
		hashtab[hashval] = np;
	} else			/* already there */
		free((void *) np->value);	/*free previous value */
	if ((np->value = strdup(value)) == NULL)
		return NULL;
	return np;
}

static int strcomp(const void *p1, const void *p2)
{
	return strcmp(* (char * const *) p1, * (char * const *) p2);
}

__DDCFG_EXPORT__ char **all_items(void)
{
	struct nlist *np;
	int i, iitem, itemcount = 0;
	char **items;

	/* first count how many items we have in the db */
	for (i = 0; i < HASHSIZE; i++) {
		np = hashtab[i];
		while (np != NULL) {
			itemcount += 1;
			np = np->next;
		}
	}

	iitem = 0;
	items = (char **) malloc((itemcount + 1) * sizeof(char*));
	for (i = 0; i < itemcount + 1; i++)
		items[i] = NULL;

	for (i = 0; i < HASHSIZE; i++) {
		np = hashtab[i];
		while (np != NULL) {
			items[iitem++] = np->key;
			np = np->next;
		}
	}

	qsort(items, iitem, sizeof(char*), strcomp);
	return items;
};

/* get all the keys/values in a string */
__DDCFG_EXPORT__ char **getall(void){
	struct nlist *np;
	int i, iitem, itemcount = 0;
	char **items;
	char *buffer;
	size_t bufferlen;

	/* first count how many items we have in the db */
	for (i = 0; i < HASHSIZE; i++) {
		np = hashtab[i];
		while (np != NULL) {
			itemcount += 1;
			np = np->next;
		}
	}

	iitem = 0;
	items = malloc((itemcount + 1) * sizeof(char*));
	for (i = 0; i < itemcount + 1; i++)
		items[i] = NULL;

	for (i = 0; i < HASHSIZE; i++) {
		np = hashtab[i];
		while (np != NULL) {
			bufferlen = 1+snprintf(NULL, 0, "%s = %s\n", np->key, np->value);
			buffer = malloc(sizeof(char) * (bufferlen));
			snprintf(buffer, bufferlen, "%s = %s\n", np->key, np->value);
			items[iitem++] = buffer;
			np = np->next;
		}
	}

	qsort(items, iitem, sizeof(char*), strcomp);
	return items;
}

/* free all the keys defined */
__DDCFG_EXPORT__ void freeall(void){
	struct nlist *np, *next;
	int i;

	for (i = 0; i < HASHSIZE; i++) {
		np = hashtab[i];
		while (np != NULL) {
			next = np->next;
			free(np->value);
			free(np->key);
			free(np);
			np = next;
		}
		np = NULL;
	}
}
