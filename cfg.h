int cfg_int(const char*, const char*);
double cfg_double(const char*, const char*);
char * cfg_get(const char*, const char*);
int cfg_bool(const char * section, const char * option);
int cfg_parse();
int cfg_parse_args(int, char **);
