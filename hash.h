#ifndef __HASH_H__
#define __HASH_H__

<<<<<<< HEAD
#define STATUS_CACHED 0x01
#define STATUS_CHECKED 0x02
=======
#ifndef __DDCFG_EXPORT__
#define __DDCFG_EXPORT__ extern
#endif
>>>>>>> Add working version of the python mockup, but in C

struct nlist {
	struct nlist *next;
	char *key;
	char *value;
	int status;
	union {
		double d_cache;
		int i_cache;
		long l_cache;
	};
};
#define HASHSIZE 1024*16

__DDCFG_EXPORT__ struct nlist *lookup(const char *);
__DDCFG_EXPORT__ struct nlist *install(const char *, const char *);
__DDCFG_EXPORT__ char **getall();
__DDCFG_EXPORT__ char **all_items(void);
__DDCFG_EXPORT__ void freeall(void);

#endif
