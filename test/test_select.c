/* ybt
 * name   : SelectFunction
 * run    : ./test_select Tests/select.ini
 * return : 0
*/
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

	int val;

	val = ddcfg_select("Section", "Language", 2, "EN", "ES");
	if (val != 0) {
		printf("Error selecting the language from the Section.Language key\n");
		exit(EXIT_FAILURE);
	}

	val = ddcfg_select("Section", "Country", 2, "EN", "ES");
	if (val != 1) {
		printf("Error selecting the country from the Section.Country key\n");
		exit(EXIT_FAILURE);
	}

	val = ddcfg_select("Section", "Country", 2, "DO", "NOT");
	if (val != -1) {
		printf("Error selecting the country from the Section.Country key\n");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
};
