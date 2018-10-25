#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "hash.h"
#include "ini.h"
#include "ddcfg.h"
#include "spec.h"

int ddcfg_errno = DDCFG_ERRNO_NOTHING;
char* ddcfg_errstr = NULL;

static char *errorNotFound = "ddcfg:Error reading %s.%s: not found\n";
static char *errorNotParse = "ddcfg:Error casting %s.%s to type %s: '%s' not parseable\n";

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
};

int handler(const char *section, const char *option, const char *value)
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

struct nlist * ddcfg_lookup(const char *section, const char *option)
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
	return search;
};

int ddcfg_parse(const char *filename)
{
	FILE *f;
	int error;
	f = fopen(filename, "r");
	if (f == NULL)
		return -1;
	error = ini_parse_file(f, handler);
	fclose(f);
	return error;
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
		}
	};
	return 0;
};

int ddcfg_select(const char *section, const char *option, int n, ...)
{
	char * value, * check;
	va_list options;
	int i;

	value = ddcfg_get(section, option);

	va_start(options, n);

	for (i = 0; i < n; i++) {
		check = va_arg(options, char*);
		if (strncmp(value, check, strlen(value)) == 0) {
			return i;
		}
	}

	va_end(options);
	return -1;
};

char *ddcfg_get(const char *section, const char *option)
{
	struct nlist * entry = ddcfg_lookup(section, option);
	return entry->value;
};

int ddcfg_parse_double(const char *string, double *value)
{
	char *ptr;
	ptr = (char *) string;
	*value = strtod(string, &ptr);
	if (ptr == string)
		return 1;
	return 0;
};

double ddcfg_double_(const char *section, const char *option, long int slen, long int olen)
{
	char *newsection, *newoption;
	double val;

	newsection = (char*) malloc(slen+1);
	newoption = (char*) malloc(olen+1);

	strncpy(newsection, section, slen);
	strncpy(newoption, option, olen);

	newsection[slen] = '\0';
	newoption[olen] = '\0';

	val = ddcfg_double(newsection, newoption);

	free(newsection);
	free(newoption);

	return val;
};

double ddcfg_double(const char *section, const char *option)
{
	struct nlist * entry;
	double number;
	int err;

	entry = ddcfg_lookup(section, option);

	if (entry->status & STATUS_CACHED) {
		return entry->d_cache;
	}
	err = ddcfg_parse_double(entry->value, &number);
	if (err)
		die(section, option, "double", entry->value);
	entry->status |= STATUS_CACHED;
	entry->d_cache = number;
	return number;
};

int ddcfg_parse_int(const char *string, int *value)
{
	char *ptr;
	ptr = (char *) string;
	*value = (int) strtol(string, &ptr, 10);
	if (ptr == string || *ptr != '\0')
		return 1;
	return 0;
};

int ddcfg_int_(const char *section, const char *option, long int slen, long int olen)
{
	char *newsection, *newoption;
	int val;

	newsection = (char*) malloc(slen+1);
	newoption = (char*) malloc(olen+1);

	strncpy(newsection, section, slen);
	strncpy(newoption, option, olen);

	newsection[slen] = '\0';
	newoption[olen] = '\0';

	val = ddcfg_int(newsection, newoption);

	free(newsection);
	free(newoption);

	return val;
};

int ddcfg_int(const char *section, const char *option)
{
	struct nlist * entry;
	int number;
	int err;

	entry = ddcfg_lookup(section, option);

	if (entry->status & STATUS_CACHED) {
		return entry->i_cache;
	}
	err = ddcfg_parse_int(entry->value, &number);
	if (err)
		die(section, option, "int", entry->value);
	entry->status |= STATUS_CACHED;
	entry->i_cache = number;
	return number;
};

int ddcfg_parse_bool(const char *string, int *value)
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

int ddcfg_bool_(const char *section, const char *option, long int slen, long int olen)
{
	char *newsection, *newoption;
	int val;

	newsection = (char*) malloc(slen+1);
	newoption = (char*) malloc(olen+1);

	strncpy(newsection, section, slen);
	strncpy(newoption, option, olen);

	newsection[slen] = '\0';
	newoption[olen] = '\0';

	val = ddcfg_bool(newsection, newoption);

	free(newsection);
	free(newoption);

	return val;
};

int ddcfg_bool(const char *section, const char *option)
{
	struct nlist * entry;
	int value, err;

	entry = ddcfg_lookup(section, option);
	if (entry->status & STATUS_CACHED) {
		return entry->i_cache;
	}
	err = ddcfg_parse_bool(entry->value, &value);
	if (err)
		die(section, option, "bool", entry->value);
	entry->status |= STATUS_CACHED;
	entry->i_cache = value;
	return value;
};

char ** ddcfg_parselist(const char *string, int *length)
{
	char *ptr, *stringcopy, *element, *chop;
	char ** list;
	int len;

	list = malloc(1024 * sizeof(char*));
	len = 0;

	stringcopy = strdup(string);
	ptr = (char *) stringcopy;
	while (ptr) {
		element = strsep(&ptr, ",");
		while (isspace(*element))
			element++;
		chop = element;
		while ((*chop) != '\0') chop++;
		while (chop > element && isspace(*--chop)) {
			*chop = '\0';
		}
		list[len] = strdup(element);
		len += 1;
	};

	*length = len;
	free(stringcopy);
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

void ddcfg_dump(const char *header, FILE *fout)
{
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

void ddcfg_free()
{
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

int ddcfg_check_spec()
{
	int err = DDCFG_ERRNO_NOTHING;

	err = spec_check();

	return err;
};

int ddcfg_load_specfile(const char *specfile)
{
	int err = DDCFG_ERRNO_NOTHING;
	FILE * f = NULL;
	unsigned char * contents = NULL;
	size_t length = 0;
		
	f = fopen(specfile, "rb");

	fseek(f, 0, SEEK_END);
	length = ftell(f);

	contents = (unsigned char *) malloc(length + 1);
	/* TODO check if this works */

	fseek(f, 0, SEEK_SET);
	fread(contents, length, 1, f);
	/* TODO check if this works */

	fclose(f);

	contents[length] = 0;
	err = spec_new_from_data(contents, length);

	return err;
};

int ddcfg_load_specdata(unsigned char *contents, unsigned int length)
{
	int err = DDCFG_ERRNO_NOTHING;

	err = spec_new_from_data(contents, length);

	return err;
};
