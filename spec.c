#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "spec.h"

char * property_strings[] = {"string", "int", "bool", "double", "subsection"};
char * section_strings[] = {"SECTION", "SUBSECTION"};

#define BUFSIZE 256

static void set_property(struct st_spec_property *prop, char *key, char *value)
{
	if (strcmp(key, "NAME") == 0) {
		prop->name = strdup(value);
	} else if (strcmp(key, "DESCRIPTION") == 0) {
		if (prop->description) {
			int length;
			length = strlen(prop->description) + strlen(value) + 1;
			prop->description = realloc(prop->description, length);
			strcat(prop->description, " ");
			strcat(prop->description, value);
		} else {
			prop->description = strdup(value);
		}
	} else if (strcmp(key, "DEPENDS_ON") == 0) {
		prop->depends_on = strdup(value);
	} else if (strcmp(key, "POINTS_TO") == 0) {
		prop->points_to = strdup(value);
	} else if (strcmp(key, "TYPE") == 0) {
		int i;
		for (i=0; i < sizeof(property_strings) / sizeof(char*); i++) {
			if (strcmp(value, property_strings[i]) == 0) {
				prop->type = i;
			}
		};
	} else if (strcmp(key, "DEFAULT") == 0) {
		prop->defaultvalue = strdup(value);
	} else if (strcmp(key, "VALUES") == 0) {
		prop->values = strdup(value);
	} else {
		printf("Wop, key not found! <%s>\n", key);
	}
};

static void set_section(struct st_spec_section *section, char *key, char *value)
{
	if (strcmp(key, "NAME") == 0) {
		section->name = strdup(value);
	} else if (strcmp(key, "DESCRIPTION") == 0) {
		if (section->description) {
			int length;
			length = strlen(section->description) + strlen(value) + 1;
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
		printf("Wop, key not found! <%s>\n", key);
	}
};

static struct st_spec_property * new_property(struct st_spec_section *section)
{
	struct st_spec_property * prop;
	prop = section->properties;

	if (prop == NULL) {
		section->properties = (struct st_spec_property *) malloc(sizeof(struct st_spec_property));
		prop = section->properties;
		bzero(prop, sizeof(struct st_spec_property));
		prop->section = section;
		return prop;
	};

	while (prop->next != NULL)
		prop = prop->next;

	prop->next = (struct st_spec_property *) malloc(sizeof(struct st_spec_property));
	bzero(prop->next, sizeof(struct st_spec_property));
	prop->next->section = section;
	return prop->next;
};

static struct st_spec_section * new_section(struct st_spec *spec)
{
	struct st_spec_section * psec;

	psec = spec->sections;

	if (psec == NULL) {
		psec = (struct st_spec_section *) malloc(sizeof(struct st_spec_section));
		spec->sections = psec;
		bzero(psec, sizeof(struct st_spec_section));
		psec->spec = spec;
		return psec;
	}

	while (psec->next != NULL)
		psec = psec->next;

	psec->next = (struct st_spec_section *) malloc(sizeof(struct st_spec_section));
	bzero(psec->next, sizeof(struct st_spec_section));
	psec->next->spec = spec;
	return psec->next;
};

struct st_spec * new_spec_from_data(const char * data, int length)
{
	struct st_spec * spec;
	spec = (struct st_spec *) malloc(sizeof(struct st_spec));
	spec->sections = NULL;
	spec->contents = data;
	spec->length = length;
	return spec;
};

struct st_spec * new_spec_from_file(const char * path)
{
	struct st_spec * spec;
	FILE * f;
	char * contents;
	int filesize;

	spec = (struct st_spec *) malloc(sizeof(struct st_spec));
	spec->sections = NULL;

	f = fopen(path, "r");

	if (f == NULL) {
		printf("Impossible to open \"%s\" for reading\n", path);
		exit(1);
	}

	contents = calloc(1024 * 1024, sizeof(char));

	filesize = fread(contents, sizeof(char), 1024 * 1024, f);

	if (filesize == 1024 * 1024) {
		printf("Max length of spec file reached!\n");
		exit(1);
	} else if (filesize == 0) {
		printf("Impossible to read spec file, or empty\n");
		exit(1);
	}
	fclose(f);

	spec->contents = contents;
	spec->length = filesize;

	return spec;
};

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

static void print_spec(struct st_spec *spec)
{
	struct st_spec_section * section;
	printf("#### BEGIN SPEC ####\n");
	section = spec->sections;
	while (section) {
		print_section(section);
		section = section->next;
	}
	printf("#### END SPEC ####\n");
};

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

int parse_spec(struct st_spec *spec)
{
	enum parser_status status;
	struct st_spec_section * section = NULL;
	struct st_spec_property * prop = NULL;
	char line[BUFSIZE], *p, *key, *value;
	FILE * stream;
	int index;

	status = NONE;
	stream = fmemopen(spec->contents, spec->length, "r");

	while (fgets(line, BUFSIZE, stream)) {
		p = line;
		while (*p && isblank(*p)) p++;
		if (*p == '#' || *p == '\n')
			continue;

		key = strtok(p, " ");
		value = strtok(NULL, "\n");

		if (is_section(key)) {
			section = new_section(spec);
			set_section(section, "TYPE", key);
			set_section(section, "NAME", value);
			status = ONSECTION;
			continue;
		} else if (is_property(key)) {
			prop = new_property(section);
			set_property(prop, "NAME", value);
			status = ONPROPERTY;
			continue;
		};

		if (status == ONPROPERTY)
			set_property(prop, key, value);
		else if (status == ONSECTION)
			set_section(section, key, value);
		else {
			printf("Found key-value outside section or property!\n");
			exit(1);
		}
	}

	fclose(stream);
}
