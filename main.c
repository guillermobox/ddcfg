#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ddcfg.h"


int main(int argc, char *argv[])
{
	ddcfg_parse("input_test.in");
	if (ddcfg_parse_args(argc, argv)) {
		printf("Error when parsing command line\n");
		exit(EXIT_FAILURE);
	};

	printf("Masa para X en double: %lf\n", ddcfg_double("QC","massX"));
	printf("Masa para Y en double: %lf\n", ddcfg_double("QC","massY"));
	printf("Masa para X en string: [%s]\n", ddcfg_get("QC","massX"));
	printf("Masa para Y en string: [%s]\n", ddcfg_get("QC","massY"));
	printf("QC.activate en string: [%s]\n", ddcfg_get("QC", "activate"));
	printf("QC.activate en bool: [%d]\n", ddcfg_bool("QC", "activate"));

//	printf("QC.not_exist: [%s]\n", ddcfg_get("QC", "not_exist"));
	printf("QC.activate en double: [%lf]\n", ddcfg_double("QC", "activate"));

	return EXIT_SUCCESS;
};
