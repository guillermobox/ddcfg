#ifndef __SPEC_H_
#define __SPEC_H_

#ifndef __DDCFG_EXPORT__
#define __DDCFG_EXPORT__ extern
#endif

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

	char * values;
	char * defaultvalue;

	struct st_spec_property * depends_on;
	struct st_spec_property * points_to;
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
