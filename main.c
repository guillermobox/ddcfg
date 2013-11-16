#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ddcfg.h"

int main(int argc, char *argv[])
{
	char *filepath = "input_test.in";
	int errno;

	if ((errno = ddcfg_parse(filepath)) != 0) {
		printf("Error when reading line %d of %s\n", errno, filepath);
		exit(EXIT_FAILURE);
	}
	if (ddcfg_parse_args(argc, argv)) {
		printf("Error when parsing command line\n");
		exit(EXIT_FAILURE);
	}

	printf("QC.massX [%s] (%lf)\n", ddcfg_get("QC", "massX"), ddcfg_double("QC", "massX"));
	printf("QC.massY [%s] (%lf)\n", ddcfg_get("QC", "massY"), ddcfg_double("QC", "massY"));
	printf("QC.activate: [%s] (%d)\n", ddcfg_get("QC", "activate"), ddcfg_bool("QC", "activate"));
	printf("alone option: [%s]\n", ddcfg_get(NULL, "alone"));

	return EXIT_SUCCESS;
};
