#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "ini.h"

static int handler( void* configuration,
		const char * section, const char * option, const char * value){

	char * new_str;
	new_str = malloc(strlen(section) + strlen(option) + 2);
	sprintf(new_str, "%s.%s", section, option);
	install(new_str, (char*)value, strlen(value)+1);
};

int cfg_parse(){
	ini_parse("input_test.in", handler, NULL);
};

int cfg_parse_args(int argc, char *argv[]){
	int i, parsing_key, parsing_value;
	char *key, *value;

	parsing_key = 0;
	parsing_value = 0;
	for(i=0; i<argc; i++){
		if( strcmp(argv[i], "--config")==0 ){
			if( i+2 >= argc ){
				return 1;
			}
			key = argv[++i];
			value = argv[++i];
			install(key, value, strlen(value)+1);
		};
	};
	return 0;
};

char * cfg_get(const char * section, const char * option){
	struct nlist * search;
	char *new_str = malloc(strlen(section) + strlen(option) + 2);
	sprintf(new_str, "%s.%s", section, option);
	search = lookup(new_str);
	if( search==NULL )
		return NULL;
	return (char*)search->value;
};

double cfg_double(const char * section, const char * option){
	char * answer, *ptr;
	double number;

	answer = cfg_get(section, option);
	if(answer==NULL){
		exit(0);
	}

	ptr = answer;
	number = strtod(answer, &ptr);
	if(ptr==answer){
		exit(0);
	}else{
		return number;
	}
};
