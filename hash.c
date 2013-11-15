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
	void * value;
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
struct nlist *install(char *name, char *defn, size_t size)
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
	np->value = malloc(size);
	if (memcpy(np->value, defn, size) == NULL)
	return np;
}

/*
int main(){
	install("QC.massX", "1.23", 5);
	install("QC.massY", "23.12342", 8);

	struct nlist * search;
	search = lookup("QC.massX");
	puts((char*)search->value);
	search = lookup("QC.massY");
	puts((char*)search->value);
};
*/
