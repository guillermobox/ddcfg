#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

typedef struct st_string {
    char * content;
    size_t len;
    size_t size;
} * String;

String string_new_empty();
String string_new_array(char *a);
void string_cat_array(String dst, const char *a);
void string_grow(String s, size_t newsize);
void string_printf(String dst, const char *fmt, ... );
void string_free(String s);