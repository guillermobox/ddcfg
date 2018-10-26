#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../ddcfg.h"

int main(int argc, char *argv[])
{
	int errno;

	if ((errno = ddcfg_parse(argv[1])) != 0) {
		if (errno == -1)
			printf("Error reading configuration file %s\n", argv[1]);
		else
			printf("%s:%d:Parsing error\n", argv[1], errno);
		exit(EXIT_FAILURE);
	}

	FILE * f = fopen(argv[1], "r");

	if (f == NULL){printf("WOP\n");}
	char * line = NULL;
	size_t linelen = 0;

	getline(&line, &linelen, f);
	while (getline(&line, &linelen, f) != -1) {
		char * equal = strchr(line, '=');
		char * end = strchr(line, '\n');
		char * key, * value;

		key = line;

		*equal = '\0';
		value = equal + 1;
		equal = equal - 1;
		while (isspace(*equal) && equal > key) {
			*equal = '\0';
			equal -= 1;
		}
		while (isspace(*key) && key < equal)
			key = key + 1;
		while (isspace(*end) && end > value) {
			*end = '\0';
			end -= 1;
		}
		while (isspace(*value) && value < end) {
			value = value + 1;
		}


		double expected, stored;

		expected = strtod(value, &value);
		stored = ddcfg_double("Items", key);

		if (expected != stored) {
			fprintf(stdout, "Error here! %.16lf != %.16lf\n", expected, stored);
			return EXIT_FAILURE;
		}
	}

	fclose(f);
	free(line);


	/* I'll do it again to check the cache */
	f = fopen(argv[1], "r");

	if (f == NULL){printf("WOP\n");}
	line = NULL;
	linelen = 0;

	getline(&line, &linelen, f);
	while (getline(&line, &linelen, f) != -1) {
		char * equal = strchr(line, '=');
		char * end = strchr(line, '\n');
		char * key, * value;

		key = line;

		*equal = '\0';
		value = equal + 1;
		equal = equal - 1;
		while (isspace(*equal) && equal > key) {
			*equal = '\0';
			equal -= 1;
		}
		while (isspace(*key) && key < equal)
			key = key + 1;
		while (isspace(*end) && end > value) {
			*end = '\0';
			end -= 1;
		}
		while (isspace(*value) && value < end) {
			value = value + 1;
		}


		double expected, stored;

		expected = strtod(value, &value);
		stored = ddcfg_double("Items", key);

		if (expected != stored) {
			fprintf(stdout, "Error here at the second pass! %.16lf != %.16lf\n", expected, stored);
			return EXIT_FAILURE;
		}
	}

	fclose(f);
	free(line);

	ddcfg_free();
	return EXIT_SUCCESS;
};
