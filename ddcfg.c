#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "ini.h"

static char *errorNotFound = "ddcfg:Error when reading %s.%s: not found\n";
static char *errorNotParse =
    "ddcfg:Error when casting %s.%s to type %s: '%s' not parseable\n";

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

static int handler(void *configuration, const char *section,
		   const char *option, const char *value)
{
	char *newstr;

	if (strlen(section) == 0) {
		install(option, value);
	} else {
		newstr = malloc(strlen(section) + strlen(option) + 2);
		sprintf(newstr, "%s.%s", section, option);
		install(newstr, value);
		free(newstr);
	}
	return 1; /* inih needs a non-zero return on success (???) */
};

int ddcfg_parse(const char *filename)
{
	return ini_parse(filename, handler, NULL);
};

int ddcfg_parse_args(int argc, char *argv[])
{
	char *key, *value;
	int i;

	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--config") == 0) {
			if (i + 2 >= argc)
				return -1;	/* return error */
			key = argv[++i];
			value = argv[++i];
			install(key, value);
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

double ddcfg_double(const char *section, const char *option)
{
	char *answer, *ptr;
	double number;

	answer = ddcfg_get(section, option);
	ptr = answer;
	number = strtod(answer, &ptr);
	if (ptr == answer)
		die(section, option, "double", answer);
	return number;
};

int ddcfg_int(const char *section, const char *option)
{
	char *answer, *ptr;
	int number;

	answer = ddcfg_get(section, option);
	ptr = answer;
	number = (int) strtol(answer, &ptr, 10);
	if (ptr == answer)
		die(section, option, "int", answer);
	return number;
};

int ddcfg_bool(const char *section, const char *option)
{
	char *answer, *lower;
	int i;

	answer = ddcfg_get(section, option);
	lower = strdup(answer);
	for (i = 0; i < strlen(lower); i++) {
		if (lower[i] >= 'A' && lower[i] <= 'Z')
			lower[i] += 32;
	}
	if (strcmp(lower, "true") == 0 || strcmp(lower, "1") == 0)
		return 1;
	else if (strcmp(lower, "false") == 0 || strcmp(lower, "0") == 0)
		return 0;
	else
		die(section, option, "bool", answer);
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
