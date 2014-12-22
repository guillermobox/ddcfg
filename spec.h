#ifndef __SPEC_H_
#define __SPEC_H_

enum parser_status {NONE, ONSECTION, ONPROPERTY};
enum property_type {STRING = 0, INT, BOOL, DOUBLE, SUBSECTION};
enum section_type {PRIMARY = 0, SECONDARY};

struct st_spec {
	struct st_spec_section *sections;
	unsigned int nsections;
	char * contents;
	unsigned int length;
};

struct st_spec_section {
	char * name;
	char * description;
	enum section_type type;
	struct st_spec_section * next;
	struct st_spec_property * properties;
};

struct st_spec_property {
	struct st_spec_property * next;
	char * name;
	char * description;
	char * depends_on;
	char * points_to;
	char * values;
	char * defaultvalue;
	enum property_type type;
};

int parse_spec(struct st_spec *spec);
struct st_spec * new_spec_from_file(const char * path);
void print_property(struct st_spec_property * prop);
struct st_spec_section * lookup_section(struct st_spec *spec, const char *secname);
struct st_spec_property * lookup_property(struct st_spec *spec, const char *secname, const char *propname);

#endif
