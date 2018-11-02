#ifndef __SPEC_H_
#define __SPEC_H_

#include "ast.h"
#include "expparser.tab.h"

struct st_spec {
	char * contents;
	unsigned int length;
	
	struct st_spec_section *sections;
	struct st_spec_constraint * constraints;
};

struct st_spec_constraint {
	char * description;
	enum yytokentype type;
	const char * expression;

	struct st_spec_property * depends_on;
	struct st_ast * ast;
	struct st_spec * spec;
	struct st_spec_constraint * next;
};

struct st_spec_section {
	char * name;
	char * description;
	enum yytokentype type;

	struct st_spec * spec;
	struct st_spec_section * next;
	struct st_spec_property * properties;
};

struct st_spec_property {
	char * name;
	char * description;
	enum yytokentype type;
	unsigned int specline;

	char * values;
	char * defaultvalue;

	struct st_spec_property * depends_on;
	struct st_spec_section * points_to;
	struct st_spec_property * next;
	struct st_spec_section * section;
};

int parse_spec(struct st_spec *spec);
void free_spec(struct st_spec * spec);
void dump_spec(struct st_spec *spec);

int spec_new_from_data(unsigned char * data, unsigned int length);

struct st_spec_section * lookup_section(struct st_spec *spec, const char *secname);
struct st_spec_property * lookup_property(struct st_spec *spec, const char *secname, const char *propname);
int spec_check_constraint(struct st_spec_constraint * constraint);
int spec_check_type(struct st_spec_property *property, char * value);
int spec_internal_consistency(struct st_spec_section *section, struct st_spec_property *property);
int spec_check_property(struct st_spec_section *section, struct st_spec_property *property, const char *secname);
int spec_check_subsection(struct st_spec_section *section, const char *secname);
int spec_check_section(struct st_spec_section *section);
int spec_check();

#endif
