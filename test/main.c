#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../ddcfg.h"

int main(int argc, char *argv[])
{
	int errno;

	if ((errno = ddcfg_parse(argv[1])) != 0) {
		printf("Error when reading line %d of %s\n", errno, argv[1]);
		exit(EXIT_FAILURE);
	}

	if (ddcfg_parse_args(argc, argv)) {
		printf("Error when parsing command line\n");
		exit(EXIT_FAILURE);
	}

	ddcfg_dump("# CONFIGURATION", stdout);

	ddcfg_free();

	return EXIT_SUCCESS;
};
