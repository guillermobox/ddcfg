#ifndef __INI_H__
#define __INI_H__

/*
 * Parse a ini file provided by a FILE descriptor. Handler is a callback
 * function that will be called for each section/name found. Handler
 * has to return 0 on success, and != 0 on error.
 *
 * This function will return 0 on success, and the line number that
 * trows an error on failure.
*/
__DDCFG_EXPORT__ int ini_parse_file(FILE* file, int (*handler)(const char* section, const char* name, const char* value));

#endif
