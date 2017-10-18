#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../ddcfg.h"

int main(int argc, char *argv[])
{
	int errno;

	if (argc < 2) {
		printf("Usage: %s <configuration> <spec>\n", argv[0]);
		exit(EXIT_FAILURE);
	};

	if ((errno = ddcfg_parse(argv[1])) != 0) {
		if (errno == -1)
			printf("Error reading configuration file %s\n", argv[1]);
		else
			printf("%s:%d:Parsing error\n", argv[1], errno);
		exit(EXIT_FAILURE);
	}

	if (argc == 3) {
		if ((errno = ddcfg_load_specfile(argv[2])) != 0) {
			if (errno == -1)
				printf("Error loading spec file %s\n", argv[2]);
			else
				printf("Error parsing line %d of spec file %s\n", errno, argv[2]);
			exit(EXIT_FAILURE);
		};

		if ((errno = ddcfg_check_spec())){
			printf("%d errors found!\n", errno);
		} else {
			printf("All clear\n");
		}

	}
	ddcfg_free();

	return EXIT_SUCCESS;
};
