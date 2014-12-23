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
unsigned hash(const char *s)
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
struct nlist *install(const char *key, const char *value)
{
	struct nlist *np;
	unsigned hashval;
	if ((np = lookup(key)) == NULL) {	/* not found */
		np = (struct nlist *) malloc(sizeof(*np));
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

char **all_items(void)
{
	struct nlist *np;
	int i, iitem;
	char **items;

	iitem = 0;
	items = malloc(1024 * sizeof(char*));
	for (i = 0; i < 1024; i++)
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
char **getall(void){
	struct nlist *np;
	int i, iitem;
	char **items;
	char buffer[128];

	iitem = 0;
	items = malloc(1024 * sizeof(char*));
	for (i = 0; i < 1024; i++)
		items[i] = NULL;

	for (i = 0; i < HASHSIZE; i++) {
		np = hashtab[i];
		while (np != NULL) {
			sprintf(buffer, "%s = %s\n", np->key, np->value);
			items[iitem++] = strdup(buffer);
			np = np->next;
		}
	}

	qsort(items, iitem, sizeof(char*), strcomp);
	return items;
}

/* free all the keys defined */
void freeall(void){
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
