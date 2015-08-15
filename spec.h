#ifndef __SPEC_H_
#define __SPEC_H_

#ifndef __DDCFG_EXPORT__
#define __DDCFG_EXPORT__ extern
#endif

enum parser_status {NONE, ONSECTION, ONPROPERTY};
enum property_type {STRING = 0, INT, BOOL, DOUBLE, SUBSECTION, PATH};
enum section_type {PRIMARY = 0, SECONDARY};

struct st_spec {
	char * contents;
	long int length;
	struct st_spec_section *sections;
};

struct st_spec_section {
	char * name;
	char * description;
	int specline;
	enum section_type type;
	struct st_spec * spec;
	struct st_spec_section * next;
	struct st_spec_property * properties;
};

struct st_spec_property {
	char * name;
	char * description;
	char * depends_on;
	char * points_to;
	char * values;
	char * defaultvalue;
	void * widget;
	int specline;
	enum property_type type;
	struct st_spec_property * next;
	struct st_spec_section * section;
};

__DDCFG_EXPORT__ int parse_spec(struct st_spec *spec);
__DDCFG_EXPORT__ void free_spec(struct st_spec * spec);
__DDCFG_EXPORT__ void dump_spec(struct st_spec *spec);
__DDCFG_EXPORT__ struct st_spec * new_spec_from_file(const char * path);
__DDCFG_EXPORT__ struct st_spec * new_spec_from_data(const char * data, int length);
__DDCFG_EXPORT__ struct st_spec_section * lookup_section(struct st_spec *spec, const char *secname);
__DDCFG_EXPORT__ struct st_spec_property * lookup_property(struct st_spec *spec, const char *secname, const char *propname);

#endif
