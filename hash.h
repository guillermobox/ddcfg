struct nlist {
	struct nlist * next;
	char * key;
	void * value;
};
struct nlist *lookup(char *s);
struct nlist *install(char *name, char *defn, size_t size);
