#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "ini.h"
#include "spec.h"
#include "ddcfg.h"

static char *errorNotFound = "ddcfg:Error reading %s.%s: not found\n";
static char *errorNotParse = "ddcfg:Error casting %s.%s to type %s: '%s' not parseable\n";
static struct st_spec * spec = NULL;

static void die(const char *section, const char *option, const char *cast,
		const char *value)
{
	if (cast == NULL || value == NULL) {
		fprintf(stderr, errorNotFound, section, option);
	} else {
		fprintf(stderr, errorNotParse, section, option, cast,
			value);
	}
	exit(EXIT_FAILURE);
}

static void spec_error(const char *errormsg, struct st_spec_property *prop)
{
	if (prop == NULL)
		fprintf(stderr, "spec:???:%s\n", errormsg);
	else
		fprintf(stderr, "spec:%d:%s.%s:%s\n", prop->specline,
			prop->section->name, prop->name, errormsg);
};

static int handler(void *configuration, const char *section,
		   const char *option, const char *value)
{
	char *newstr;
	if (strlen(section) == 0)
		return 1;
	newstr = malloc(strlen(section) + strlen(option) + 2);
	sprintf(newstr, "%s.%s", section, option);
	install(newstr, value);
	free(newstr);
	return 0;
};

int ddcfg_parse(const char *filename)
{
	FILE *f;
	int error;
	f = fopen(filename, "r");
	if (f == NULL)
		return -1;
	error = ini_parse_file(f, handler, NULL);
	fclose(f);
	return error;
};

void ddcfg_dump_spec()
{
	if (spec != NULL)
		dump_spec(spec);
};

int ddcfg_parse_args(int argc, char *argv[])
{
	char *key, *value, *path;
	int i;

	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--config") == 0) {
			if (i + 2 >= argc)
				return -1;	/* return error */
			key = argv[++i];
			value = argv[++i];
			install(key, value);
		}
		else if (strcmp(argv[i], "--check") == 0) {
			if (i + 1 >= argc)
				return -1;
			path = argv[++i];
			ddcfg_check(path);
		}
		else if (strcmp(argv[i], "--spec") == 0) {
			ddcfg_dump_spec();
		};
	};
	return 0;
};

char *ddcfg_get(const char *section, const char *option)
{
	struct nlist *search;
	char *newstr;

	if (section == NULL || strlen(section) == 0) {
		search = lookup(option);
	} else {
		newstr = malloc(strlen(section) + strlen(option) + 2);
		sprintf(newstr, "%s.%s", section, option);
		search = lookup(newstr);
		free(newstr);
	}
	if (search == NULL)
		die(section, option, NULL, NULL);
	return search->value;
};

static int ddcfg_parse_double(const char *string, double *value)
{
	char *ptr;
	ptr = (char *) string;
	*value = strtod(string, &ptr);
	if (ptr == string)
		return 1;
	return 0;
};

double ddcfg_double(const char *section, const char *option)
{
	char *answer;
	double number;
	int err;

	answer = ddcfg_get(section, option);
	err = ddcfg_parse_double(answer, &number);
	if (err)
		die(section, option, "double", answer);
	return number;
};

static int ddcfg_parse_int(const char *string, int *value)
{
	char *ptr;
	ptr = (char *) string;
	*value = (int) strtol(string, &ptr, 10);
	if (ptr == string || *ptr != '\0')
		return 1;
	return 0;
};

int ddcfg_int(const char *section, const char *option)
{
	char *answer;
	int number;
	int err;

	answer = ddcfg_get(section, option);
	err = ddcfg_parse_int(answer, &number);
	if (err)
		die(section, option, "int", answer);
	return number;
};

static int ddcfg_parse_bool(const char *string, int *value)
{
	char *lower;
	int i;

	lower = strdup(string);
	for (i = 0; i < strlen(lower); i++) {
		if (lower[i] >= 'A' && lower[i] <= 'Z')
			lower[i] += 32;
	}
	if (strcmp(lower, "true") == 0 ||
			strcmp(lower, "on") == 0 ||
			strcmp(lower, "yes") == 0 ||
			strcmp(lower, "1") == 0) {
		*value = 1;
	}else if (strcmp(lower, "false") == 0 ||
			strcmp(lower, "off") == 0 ||
			strcmp(lower, "no") == 0 ||
			strcmp(lower, "0") == 0) {
		*value = 0;
	} else {
		free(lower);
		return 1;
	}

	free(lower);
	return 0;
};

int ddcfg_bool(const char *section, const char *option)
{
	char *answer;
	int value, err;

	answer = ddcfg_get(section, option);
	err = ddcfg_parse_bool(answer, &value);

	if (err)
		die(section, option, "bool", answer);

	return value;
};

char ** ddcfg_parselist(const char *string, int *length)
{
	char *ptr, *element, *chop;
	char ** list;
	int len;

	list = malloc(1024 * sizeof(char*));
	len = 0;

	ptr = (char *) string;
	while (ptr) {
		element = strsep(&ptr, ",");
		while (*element == ' ')
			element++;
		chop = element;
		while ((*chop) != '\0') chop++;
		while ((*(--chop)) == ' ')
			*chop = '\0';
		list[len] = strdup(element);
		len += 1;
	};

	*length = len;
	return list;
};

char** ddcfg_getlist(const char *section, const char *option, int *length)
{
	char *answer;
	char **list;

	answer = strdup(ddcfg_get(section, option));
	list = ddcfg_parselist(answer, length);
	free(answer);

	return list;
};

void ddcfg_dump(const char *header, FILE *fout){
	char **items;
	int i;
	
	if (header != NULL )
		fprintf(fout, "%s\n", header);

	items = getall();
	for (i = 0; items[i] != NULL; i++) {
		fputs(items[i], fout);
		free(items[i]);
	}

	if (header != NULL )
		fprintf(fout, "%s\n", header);

	free(items);
};

void ddcfg_free(){
	freeall();
};

char * ddcfg_is_defined(const char *section, const char *option)
{
	struct nlist *search;
	char *newstr;

	if (section == NULL || strlen(section) == 0) {
		search = lookup(option);
	} else {
		newstr = malloc(strlen(section) + strlen(option) + 2);
		sprintf(newstr, "%s.%s", section, option);
		search = lookup(newstr);
		free(newstr);
	}
	if (search)
		return search->value;
	else
		return NULL;
};


static int ddcfg_check_property(struct st_spec_section *section, struct st_spec_property *property, const char *secname);

static int checked_list(const char * section, const char * property)
{
	static char ** list = NULL;
	static int length = 0;

	if (list == NULL) {
		list = calloc(1024, sizeof(char*));
	}

	if (section == NULL && property == NULL) {
		char **item, **visited;
		int found;
		int errors = 0;
		item = all_items();

		while (*item) {
			found = 0;
			visited = list;
			while (*visited) {
				if (strcmp(*visited, *item) == 0) {
					found = 1;
					break;
				}
				visited++;
			};

			if (found == 0) {
				fprintf(stderr, "spec:??:%s:Property not found in spec\n", *item);
				errors += 1;
			};

			item++;
		};

		return errors;
	} else {
		list[length] = malloc(strlen(section) + strlen(property) + 2);
		sprintf(list[length], "%s.%s", section, property);
		length++;
	}
	return 0;
};

static int ddcfg_check_subsection(struct st_spec_section *section, const char *secname)
{
	struct st_spec_property * property;
	int err = 0;

	property = section->properties;
	while (property) {
		err += ddcfg_check_property(section, property, secname);
		property = property->next;
	};
	return err;
};

static int ddcfg_check_property(struct st_spec_section *section, struct st_spec_property *property, const char *secname)
{
	const char * value;
	int err;

	checked_list(secname, property->name);
	value = ddcfg_is_defined(secname, property->name);

	if (value == NULL) {
		if (property->defaultvalue) {
			handler(NULL, secname, property->name, property->defaultvalue);
			value = property->defaultvalue;
			spec_error("Using default value", property);
		} else {
			if (property->depends_on) {
				struct nlist *search;
				int boolvalue;
				search = lookup(property->depends_on);
				if (search) {
					err = ddcfg_parse_bool(search->value, &boolvalue);
					if (err == 0 && boolvalue == 0) return 0;
				} else {
					struct st_spec_property * remote;
					char * remotefull = strdup(property->depends_on);
					char *remotesec, *remoteprop;

					remotesec = strtok(remotefull, ".");
					remoteprop = strtok(NULL, ".");

					remote = lookup_property(spec, remotesec, remoteprop);
					free(remotefull);

					if (remote && remote->defaultvalue) {
						err = ddcfg_parse_bool(remote->defaultvalue, &boolvalue);
						if (err == 0 && boolvalue == 0) return 0;
					}
				}
			}
			spec_error("Property not found", property);
			return 1;
		}
	}

	if (property->type == INT) {
		int tmp;
		err = ddcfg_parse_int(value, &tmp);
		if (err) {
			spec_error("Property does not parse to int", property);
			return 1;
		}
	} else if (property->type == DOUBLE) {
		double tmp;
		err = ddcfg_parse_double(value, &tmp);
		if (err) {
			spec_error("Property does not parse to double", property);
			return 1;
		}
	} else if (property->type == BOOL) {
		int tmp;
		err = ddcfg_parse_bool(value, &tmp);
		if (err) {
			spec_error("Property does not parse to bool", property);
			return 1;
		}
	} else if (property->type == SUBSECTION) {
		char ** sections;
		int length, i, err = 0;

		sections = ddcfg_getlist(secname, property->name, &length);

		for (i = 0; i < length; i++) {
			struct st_spec_section * remote;
			remote = lookup_section(spec, property->points_to);
			err = ddcfg_check_subsection(remote, sections[i]);
		};
		return err;
	};

	if (property->values) {
		char ** possible_values;
		int i, number_values;

		possible_values = ddcfg_parselist(property->values, &number_values);

		for (i = 0; i < number_values; i++) {
			if (strcmp(possible_values[i], value) == 0)
				break;
		};

		if (i == number_values) {
			spec_error("Property does not match with value list", property);
			return 1;
		}
	};
	return 0;
}

static int ddcfg_check_section(struct st_spec_section *section)
{
	struct st_spec_property * property;
	int err = 0;

	property = section->properties;
	while (property) {
		err += ddcfg_check_property(section, property, section->name);
		property = property->next;
	};
	return err;
}

int ddcfg_check(const char *specfile)
{
	struct st_spec_section * section;
	int err = 0;

	spec = new_spec_from_file(specfile);
	err = parse_spec(spec);
	
	if (err) {
		return -err;
	}

	section = spec->sections;
	while (section) {
		if (section->type == PRIMARY)
			err += ddcfg_check_section(section);
		section = section->next;
	}

	err += checked_list(NULL, NULL);

	return err;
};

