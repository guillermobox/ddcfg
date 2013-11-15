#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ini.h"
#include "hash.h"
#include "cfg.h"


int main(int argc, char *argv[]){

	cfg_parse();
	if( cfg_parse_args(argc, argv) ){
		printf("Error when parsing command line\n");
		exit(EXIT_FAILURE);
	};

	printf("Masa para X en double: %lf\n", cfg_double("QC","massX"));
	printf("Masa para Y en double: %lf\n", cfg_double("QC","massY"));
	printf("Masa para X en string: [%s]\n", cfg_get("QC","massX"));
	printf("Masa para Y en string: [%s]\n", cfg_get("QC","massY"));

	return EXIT_SUCCESS;
}
