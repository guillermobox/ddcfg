#ifndef __SPEC_H_
#define __SPEC_H_

enum parser_status {NONE, ONSECTION, ONPROPERTY};
enum property_type {STRING = 0, INT, BOOL, DOUBLE, SUBSECTION};
enum section_type {PRIMARY = 0, SECONDARY};

struct st_spec {
	char * contents;
	unsigned int length;
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
	int specline;
	enum property_type type;
	struct st_spec_property * next;
	struct st_spec_section * section;
};

int parse_spec(struct st_spec *spec);
void print_spec(struct st_spec *spec, const char * header);
void delete_spec(struct st_spec * spec);
void dump_spec(struct st_spec *spec);
struct st_spec * new_spec_from_file(const char * path);
struct st_spec * new_spec_from_data(const char * data, int length);
struct st_spec_section * lookup_section(struct st_spec *spec, const char *secname);
struct st_spec_property * lookup_property(struct st_spec *spec, const char *secname, const char *propname);

#endif
