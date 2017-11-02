#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "spec.h"

char * type_strings[] = {"string", "int", "bool", "double", "subsection"};
char * section_strings[] = {"SECTION", "SUBSECTION"};

static int set_property(struct st_spec_property *prop, char *key, char *value)
{
	if (strcmp(key, "NAME") == 0) {
		if (prop->name) return 1;
		prop->name = strdup(value);
	} else if (strcmp(key, "DESCRIPTION") == 0) {
		if (prop->description) {
			int length;
			length = strlen(prop->description) + strlen(value) + 2;
			prop->description = realloc(prop->description, length);
			strcat(prop->description, " ");
			strcat(prop->description, value);
		} else {
			prop->description = strdup(value);
		}
	} else if (strcmp(key, "DEPENDS_ON") == 0) {
		if (prop->depends_on) return 1;
		prop->depends_on = strdup(value);
	} else if (strcmp(key, "POINTS_TO") == 0) {
		if (prop->points_to) return 1;
		prop->points_to = strdup(value);
	} else if (strcmp(key, "TYPE") == 0) {
		int i;
		for (i=0; i < sizeof(type_strings) / sizeof(char*); i++) {
			if (strcmp(value, type_strings[i]) == 0) {
				prop->type = i;
				return 0;
			}
		};
		return 1;
	} else if (strcmp(key, "DEFAULT") == 0) {
		if (prop->defaultvalue) return 1;
		prop->defaultvalue = strdup(value);
	} else if (strcmp(key, "VALUES") == 0) {
		if (prop->values) return 1;
		prop->values = strdup(value);
	} else {
		return 1;
	}
	return 0;
};

static int set_section(struct st_spec_section *section, char *key, char *value)
{
	if (strcmp(key, "NAME") == 0) {
		if (section->name) return 1;
		section->name = strdup(value);
	} else if (strcmp(key, "DESCRIPTION") == 0) {
		if (section->description) {
			int length;
			length = strlen(section->description) + strlen(value) + 2;
			section->description = realloc(section->description, length);
			strcat(section->description, " ");
			strcat(section->description, value);
		} else {
			section->description = strdup(value);
		}
	} else if (strcmp(key, "TYPE") == 0) {
		int i;
		for (i=0; i < sizeof(section_strings) / sizeof(char*); i++) {
			if (strcmp(value, section_strings[i]) == 0) {
				section->type = i;
			}
		};
	} else {
		return 1;
	}
	return 0;
};

static struct st_spec_property * new_property(struct st_spec_section *section, const char * name)
{
	struct st_spec_property * prop;
	prop = section->properties;

	if (prop == NULL) {
		section->properties = (struct st_spec_property *) malloc(sizeof(struct st_spec_property));
		prop = section->properties;
		bzero(prop, sizeof(struct st_spec_property));
		prop->section = section;
		prop->name = strdup(name);
		return prop;
	};

	while (prop->next != NULL) {
		if (strcmp(prop->name, name) == 0) {
			return NULL;
		}
		prop = prop->next;
	}

	if (strcmp(prop->name, name) == 0) {
		return NULL;
	}

	prop->next = (struct st_spec_property *) malloc(sizeof(struct st_spec_property));
	bzero(prop->next, sizeof(struct st_spec_property));
	prop->next->section = section;
	prop->next->name = strdup(name);
	return prop->next;
};

static struct st_spec_section * new_section(struct st_spec *spec, const char * name)
{
	struct st_spec_section * psec;

	psec = spec->sections;

	if (psec == NULL) {
		psec = (struct st_spec_section *) malloc(sizeof(struct st_spec_section));
		spec->sections = psec;
		bzero(psec, sizeof(struct st_spec_section));
		psec->spec = spec;
		psec->name = strdup(name);
		return psec;
	}

	while (psec->next != NULL) {
		if (strcmp(psec->name, name) == 0)
			return NULL;
		psec = psec->next;
	}

	if (strcmp(psec->name, name) == 0) {
		return NULL;
	}

	psec->next = (struct st_spec_section *) malloc(sizeof(struct st_spec_section));
	bzero(psec->next, sizeof(struct st_spec_section));
	psec->next->spec = spec;
	psec->next->name = strdup(name);
	return psec->next;
};

__DDCFG_EXPORT__ struct st_spec * new_spec_from_data(const char * data, int length)
{
	struct st_spec * spec;
	spec = (struct st_spec *) malloc(sizeof(struct st_spec));
	spec->sections = NULL;
	spec->contents = (char *) strdup(data);
	spec->length = length;
	return spec;
};

__DDCFG_EXPORT__ struct st_spec * new_spec_from_file(const char * path)
{
	struct st_spec * spec;
	FILE * f = NULL;
	char * contents = NULL;
	long int filesize = 0, readsize = 0;

	spec = (struct st_spec *) malloc(sizeof(struct st_spec));
	spec->sections = NULL;

	f = fopen(path, "r");

	if (f == NULL) {
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	fseek(f, 0, SEEK_SET);

	contents = malloc(filesize);
	readsize = fread(contents, sizeof(char), filesize, f);

	if (readsize != filesize) {
		free(contents);
		return NULL;
	}
	fclose(f);

	spec->contents = contents;
	spec->length = filesize;

	return spec;
};

static void free_property(struct st_spec_property *prop)
{
	if (prop->name) free(prop->name);
	if (prop->description) free(prop->description);
	if (prop->depends_on) free(prop->depends_on);
	if (prop->points_to) free(prop->points_to);
	if (prop->values) free(prop->values);
	if (prop->defaultvalue) free(prop->defaultvalue);
	if (prop->next) free_property(prop->next);
	free(prop);
};

static void free_section(struct st_spec_section * sec)
{
	if (sec->name) free(sec->name);
	if (sec->description) free(sec->description);
	if (sec->properties)
		free_property(sec->properties);
	if (sec->next) free_section(sec->next);
	free(sec);
};

__DDCFG_EXPORT__ void free_spec(struct st_spec * spec)
{
	if (spec->contents) free(spec->contents);
	free_section(spec->sections);
	free(spec);
};

/*
static void print_property(struct st_spec_property * prop)
{
	printf("PROPERTY %s\n", prop->name);
	printf("\tDESCRIPTION %s\n", prop->description);
	if (prop->type >= 0)
		printf("\tTYPE %s\n", property_strings[prop->type]);
	if (prop->defaultvalue)
		printf("\tDEFAULT %s\n", prop->defaultvalue);
	if (prop->depends_on)
		printf("\tDEPENDS_ON %s\n", prop->depends_on);
	if (prop->points_to)
		printf("\tPOINTS_TO %s\n", prop->points_to);
	if (prop->values)
		printf("\tVALUES %s\n", prop->values);
	printf("\n");
};

static void print_section(struct st_spec_section *section)
{
	struct st_spec_property * prop;
	if (section->type == PRIMARY)
		printf("SECTION %s\n", section->name);
	else if (section->type == SECONDARY)
		printf("SUBSECTION %s\n", section->name);
	printf("\tDESCRIPTION %s\n", section->description);
	printf("\n");
	prop = section->properties;
	while (prop) {
		print_property(prop);
		prop = prop->next;
	}
};

static void print_spec(struct st_spec *spec, const char * header)
{
	struct st_spec_section * section;
	printf("%s\n", header);
	section = spec->sections;
	while (section) {
		print_section(section);
		section = section->next;
	}
	printf("%s\n", header);
};
*/

static int is_section(char *key)
{
	int i;
	for (i = 0; i < sizeof(section_strings) / sizeof(char*); i++) {
		if (strcmp(key, section_strings[i]) == 0)
			return 1;
	}
	return 0;
};

static int is_property(char *key)
{
	if (strcmp(key, "PROPERTY") == 0)
		return 1;
	return 0;
}

__DDCFG_EXPORT__ struct st_spec_section * lookup_section(struct st_spec *spec, const char *secname)
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

__DDCFG_EXPORT__ struct st_spec_property * lookup_property(struct st_spec *spec, const char *secname, const char *propname)
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

__DDCFG_EXPORT__ void dump_spec(struct st_spec *spec)
{
	printf("%s", spec->contents);
};
	
__DDCFG_EXPORT__ int parse_spec(struct st_spec *spec)
{
	enum parser_status status;
	struct st_spec_section * section = NULL;
	struct st_spec_property * prop = NULL;
	char *line = NULL, *p, *key, *value;
	size_t linelen = 0;
	int lineno;
	FILE * stream;

	status = NONE;
	lineno = 0;
	stream = fmemopen(spec->contents, spec->length, "r");

	while (getline(&line, &linelen, stream) != -1) {
		lineno++;
		p = line;
		while (*p && isblank(*p)) p++;
		if (*p == '#' || *p == '\n')
			continue;

		key = strtok(p, " ");
		value = strtok(NULL, "\n");

		if (is_section(key)) {
			section = new_section(spec, value);
			if (section == NULL)
				return lineno;
			if (set_section(section, "TYPE", key)) return lineno;
			section->specline = lineno;
			status = ONSECTION;
			continue;
		} else if (is_property(key)) {
			prop = new_property(section, value);
			if (prop == NULL) {
				return lineno;
			}
			prop->specline = lineno;
			status = ONPROPERTY;
			continue;
		};

		if (status == ONPROPERTY) {
			if (set_property(prop, key, value)) return lineno;
		} else if (status == ONSECTION) {
			if (set_section(section, key, value)) return lineno;
		} else {
			return lineno;
		}
	}

	fclose(stream);
	if (line)
		free(line);
	return 0;
}
