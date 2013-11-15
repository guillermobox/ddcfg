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
	install(new_str, value);
	free(new_str);
};

int cfg_parse(){
	ini_parse("input_test.in", handler, NULL);
};

int cfg_parse_args(int argc, char *argv[]){
	char * key, * value;
	int i;

	for(i=0; i<argc; i++){
		if( strcmp(argv[i], "--config")==0 ){
			if( i+2 >= argc )
				return -1; /* return error */
			key = argv[++i];
			value = argv[++i];
			install(key, value);
		};
	};
	return 0;
};

char * cfg_get(const char * section, const char * option){
	struct nlist * search;
	char *new_str = malloc(strlen(section) + strlen(option) + 2);
	sprintf(new_str, "%s.%s", section, option);
	search = lookup(new_str);
	free(new_str);
	if( search==NULL )
		return NULL;
	return search->value;
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

int cfg_int(const char * section, const char * option){
	char * answer, *ptr;
	int number;

	answer = cfg_get(section, option);
	if(answer==NULL){
		exit(0);
	}

	ptr = answer;
	number = (int)strtol(answer, &ptr, 10);
	if(ptr==answer){
		exit(0);
	}else{
		return number;
	}
};

int cfg_bool(const char * section, const char * option){
	char * answer, *lower;
	int i;

	answer = cfg_get(section, option);
	if(answer==NULL){
		exit(0);
	}

	lower = strdup(answer);
	for(i=0; i<strlen(lower); i++){
		if( lower[i]>='A' && lower[i]<='Z' )
			lower[i] += 32;
	}


	if( strcmp(lower, "true")==0 || strcmp(lower,"1")==0 )
		return 1;
	else if( strcmp(lower, "false")==0 || strcmp(lower,"0")==0 )
		return 0;
	else
		exit(0);
};
