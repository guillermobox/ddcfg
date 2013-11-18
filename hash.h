struct nlist {
	struct nlist *next;
	char *key;
	void *value;
};
struct nlist *lookup(const char *);
struct nlist *install(const char *, const char *);
char **getall();
