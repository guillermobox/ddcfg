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

	ddcfg_load_specfile(argv[2]);
	errno = ddcfg_check_spec();

	if (errno) {
		printf("%d errors found!\n", errno);
	} else {
		printf("All clear\n");
	}

	ddcfg_free();

	return EXIT_SUCCESS;
};
