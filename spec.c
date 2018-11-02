#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "spec.h"
#include "ddcfg.h"
#include "hash.h"
#include "ini.h"


typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

struct st_spec * spec = NULL;

int spec_error_count = 0;
int spec_warning_count = 0;

static void spec_add_error()
{
	spec_error_count += 1;
};

static void spec_add_warning()
{
	spec_warning_count += 1;
};

static void spec_error(const char *errormsg, struct st_spec_property *prop)
{
	spec_add_error();
	if (prop == NULL)
		fprintf(stderr, "spec:???:%s\n", errormsg);
	else
		fprintf(stderr, "spec:%d:%s.%s:%s\n", prop->specline,
			prop->section->name, prop->name, errormsg);
};

int spec_check()
{
	struct st_spec_section * section;
	struct st_spec_constraint * constraint;

	/* check that the spec complies with the database */
	section = spec->sections;
	while (section) {
		if (section->type == T_SECTION)
			spec_check_section(section);
		section = section->next;
	}

	/* check the constraints */
	constraint = spec->constraints;
	while (constraint) {
		spec_check_constraint(constraint);
		constraint = constraint->next;
	}

	/* check that no other items in the database are unchecked */
	char **items;
	int i;
	items = all_items();
	for (i = 0; items[i] != NULL; i++) {
		struct nlist * prop;
		prop = lookup(items[i]);
		if (prop && !(prop->status & STATUS_CHECKED)) {
			fprintf(stderr, "spec:??:%s:Property not found in spec\n", items[i]);
		}
	}
	free(items);

	return spec_error_count + spec_warning_count;
};

int spec_check_type(struct st_spec_property *property, char * value)
{
	int itmp;
	double dtmp;

	switch (property->type) {
		case T_TYPE_INTEGER:
			return ddcfg_parse_int(value, &itmp);
		case T_TYPE_REAL:
			return ddcfg_parse_double(value, &dtmp);
		case T_TYPE_BOOLEAN:
			return ddcfg_parse_bool(value, &itmp);
		case T_TYPE_STRING:
			return 0;
		default:
			printf("IMPOSSIBLE!");
	};
	return 0;	
};

int spec_internal_consistency(struct st_spec_section *section, struct st_spec_property *property)
{
	int err = 0;

	if (property->defaultvalue) {
		if (spec_check_type(property, property->defaultvalue)) {
			err++;
			spec_error("Consistency problem, the default value does not parse", property);
		}
	}
	if (property->values) {
		char ** possible_values;
		int i, j, number_values;

		possible_values = ddcfg_parselist(property->values, &number_values);

		for (i = 0; i < number_values; i++) {
			if (strlen(possible_values[i]) == 0) {
				err++;
				spec_error("Consistency problem, one of the values is empty", property);
			}
			if (spec_check_type(property, possible_values[i])) {
				err++;
				spec_error("Consistency problem, one of the values does not parse", property);
			}
		}

		if (property->defaultvalue) {
			for (i = 0; i < number_values; i++) {
				if (strcmp(possible_values[i], property->defaultvalue) == 0)
					break;
			}
			if (i == number_values) {
				err++;
				spec_error("Consistency problem, the default value is not in the list of values", property);
			}

		}

		for (j = 0; j < number_values; j++)
			free(possible_values[j]);
		free(possible_values);
	}
	if (property->depends_on) {
		struct st_spec_property * remote;
		remote = property->depends_on;

		if (remote == NULL) {
			err++;
			spec_error("Consistency problem, this property points to a unknown property", property);
		} else {
			if (remote->type != T_TYPE_BOOLEAN) {
				err++;
				spec_error("Consistency problem, this property points to a non-boolean property", property);
			}
			if (remote == property) {
				err++;
				spec_error("Consistency problem, this property points to itself", property);
			}
		}
	}

	if (property->points_to) {
		struct st_spec_section * remote;
		remote = property->points_to;

		if (remote == NULL) {
			err++;
			spec_error("Consistency problem, this property points to a unknown section", property);
		} else {
			if (remote->type != T_SUBSECTION) {
				err++;
				spec_error("Consistency problem, this property points to a section which is not subsection", property);
			}
		}
	}
	return err;
};

char * fullname_from_property(struct st_spec_property * prop)
{
	char * buffer;
	size_t length;
	length = strlen(prop->name) + strlen(prop->section->name) + 2;
	buffer = malloc(length);
	snprintf(buffer, length, "%s.%s", prop->section->name, prop->name);
	return buffer;
}

static int check_if_required(struct st_spec_property * depends_on)
{
	struct st_spec_property * remote = depends_on;
	struct nlist *search;
	const char *key;
	int boolvalue, err;

	if (remote == NULL)
		return 1;
	
	key = fullname_from_property(remote);
	search = lookup(key);
	free((void*)key);

	if (search) {
		err = ddcfg_parse_bool(search->value, &boolvalue);
		if (err == 0 && boolvalue == 0) return 0;
	} else {
		if (remote->defaultvalue) {
			err = ddcfg_parse_bool(remote->defaultvalue, &boolvalue);
			if (err == 0 && boolvalue == 0) return 0;
		}
	}
	return 1;
}

int spec_check_property(struct st_spec_section *section, struct st_spec_property *property, const char *secname)
{
	const char * value, *key;
	int err;

	if ((err = spec_internal_consistency(section, property)) != 0)
		return err;

	value = ddcfg_is_defined(secname, property->name);

	if (value == NULL) {
		if (property->defaultvalue) {
			handler(secname, property->name, property->defaultvalue);
			value = property->defaultvalue;
		} else {
			if (check_if_required(property->depends_on)) {
				spec_error("Property not found", property);
				return 1;
			} else {
				return 0;
			}
		}
	}

	/* Here we found the property in the database, set to checked */
	struct nlist * item = ddcfg_lookup(secname, property->name);
	item->status |= STATUS_CHECKED;

	if (property->type == T_TYPE_INTEGER) {
		int tmp;
		err = ddcfg_parse_int(value, &tmp);
		if (err) {
			spec_error("Property does not parse to int", property);
			return 1;
		}
	} else if (property->type == T_TYPE_REAL) {
		double tmp;
		err = ddcfg_parse_double(value, &tmp);
		if (err) {
			spec_error("Property does not parse to double", property);
			return 1;
		}
	} else if (property->type == T_TYPE_BOOLEAN) {
		int tmp;
		err = ddcfg_parse_bool(value, &tmp);
		if (err) {
			spec_error("Property does not parse to bool", property);
			return 1;
		}
	} else if (property->type == T_TYPE_SUBSECTION) {
		char ** sections;
		int length, i, err = 0;

		sections = ddcfg_getlist(secname, property->name, &length);

		for (i = 0; i < length; i++) {
			struct st_spec_section * remote;
			remote = property->points_to;
			err = spec_check_subsection(remote, sections[i]);
			free(sections[i]);
		};
		free(sections);
		return err;
	};

	if (property->values) {
		char ** possible_values;
		int i, j, number_values;

		possible_values = ddcfg_parselist(property->values, &number_values);

		for (i = 0; i < number_values; i++) {
			if (strcmp(possible_values[i], value) == 0)
				break;
		};

		for (j = 0; j < number_values; j++)
			free(possible_values[j]);
		free(possible_values);

		if (i == number_values) {
			spec_error("Property does not match with value list", property);
			return 1;
		}
	};
	return 0;
}

int spec_check_subsection(struct st_spec_section *section, const char *secname)
{
	struct st_spec_property * property;
	int err = 0;

	property = section->properties;
	while (property) {
		err += spec_check_property(section, property, secname);
		property = property->next;
	};
	return err;
};


int spec_check_section(struct st_spec_section *section)
{
	struct st_spec_property * property;
	int err = 0;
	property = section->properties;
	while (property) {
		err += spec_check_property(section, property, section->name);
		property = property->next;
	};
	return err;
}

int spec_check_constraint(struct st_spec_constraint * constraint)
{
	int err = 0;
	struct st_ast_value result;

	if (check_if_required(constraint->depends_on) == 0)
		return 0;

	result = evaluate(constraint->ast);

	if ( (constraint->flavour == T_ASSERT && result.boolean != 1 ) ||
		 (constraint->flavour == T_REJECT && result.boolean != 0))
	 {
		err += 1;
		if (constraint->type == T_FAILURE)
			spec_add_error();
		else
			spec_add_warning();
	}
	if (result.type != T_TYPE_BOOLEAN) {
		err += 1;
		if (constraint->type == T_FAILURE)
			spec_add_error();
		else
			spec_add_warning();
	}
	return err;
}

int spec_new_from_data(unsigned char * data, unsigned int length)
{
	extern void bison_parse(void);
	YY_BUFFER_STATE buffer;

	spec = (struct st_spec *) malloc(sizeof(struct st_spec));

	spec->sections = NULL;
	spec->constraints = NULL;
	spec->contents = (char *) data;
	spec->length = length;

	buffer = yy_scan_string(spec->contents);

	bison_parse();

	yy_delete_buffer(buffer);

	return 0;
};

void free_spec(struct st_spec * spec)
{
	if (spec->contents) free(spec->contents);
	free(spec);
};

struct st_spec_section * lookup_section(struct st_spec *spec, const char *secname)
{
	struct st_spec_section * section;
	
	section = spec->sections;
	while (section) {
		if (strcmp(section->name, secname) == 0) {
			return section;
		}
		section = section->next;
	}
	return NULL;
};

struct st_spec_property * lookup_property(struct st_spec *spec, const char *secname, const char *propname)
{
	struct st_spec_section * section;
	struct st_spec_property * property;
	
	section = lookup_section(spec, secname);
	if (section == NULL)
		return NULL;

	property = section->properties;
	while (property) {
		if (strcmp(property->name, propname) == 0) {
			return property;
		}
		property = property->next;
	}
	return NULL;
};

void dump_spec(struct st_spec *spec)
{
	printf("%s", spec->contents);
};
