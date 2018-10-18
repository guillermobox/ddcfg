#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "spec.h"


char * property_strings[] = {"string", "int", "bool", "double", "subsection", "path"};
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

	spec->contents = contents;
	spec->length = filesize;
	return spec;
};

__DDCFG_EXPORT__ void free_spec(struct st_spec * spec)
{
	if (spec->contents) free(spec->contents);
	free(spec);
};

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
