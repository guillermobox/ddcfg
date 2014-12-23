#ifndef __HASH_H__
#define __HASH_H__

struct nlist {
	struct nlist *next;
	char *key;
	char *value;
};
#define HASHSIZE 101
struct nlist *lookup(const char *);
struct nlist *install(const char *, const char *);
char **getall();
char **all_items(void);
void freeall(void);

#endif
