#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "ini.h"
#include "spec.h"

static char *errorNotFound = "ddcfg:Error reading %s.%s: not found\n";
static char *errorNotParse = "ddcfg:Error casting %s.%s to type %s: '%s' not parseable\n";
static char *errorDefault  = "ddcfg:Warning using default value for key: %s=%s\n";

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
	char *answer, *ptr;
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
	char *answer, *ptr;
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

char** ddcfg_getlist(const char *section, const char *option, int *length)
{
	char *answer, *answerpt, *element, divisor=',', *chop;
	char **list;
	int len;

	list = malloc(1024 * sizeof(char*));
	len = 0;

	answer = strdup(ddcfg_get(section, option));
	answerpt = answer;

	while (answerpt) {
		element = strsep(&answerpt, ",");
		while (*element == ' ')
			element++;
		chop = element;
		while ((*chop) != '\0') chop++;
		while ((*(--chop)) == ' ')
			*chop = '\0';
		list[len] = strdup(element);
		len += 1;
	};

	free(answer);

	*length = len;
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

