#ifndef __HASH_H__
#define __HASH_H__

struct nlist {
	struct nlist *next;
	char *key;
	char *value;
	int cached;
	union {
		double d_cache;
		int i_cache;
		long l_cache;
	};
};
#define HASHSIZE 101

__DDCFG_EXPORT__ struct nlist *lookup(const char *);
__DDCFG_EXPORT__ struct nlist *install(const char *, const char *);
__DDCFG_EXPORT__ char **getall();
__DDCFG_EXPORT__ char **all_items(void);
__DDCFG_EXPORT__ void freeall(void);

#endif
