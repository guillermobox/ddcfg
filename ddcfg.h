/* ddcfg.h
 *
 * Header file for density-gradient configuration parser
 */

#ifndef __DDCFG_H__
#define __DDCFG_H__

/* Parse a file to the dictionary of key/value pairs. Call this
 * before any other call, otherwise they will return error.
 */
int ddcfg_parse(char *filename);

/* Parse the arguments from the command line, call it after cfg_parse to
 * mantain the priority of the command line arguments over the configuration
 * file
 */
int ddcfg_parse_args(int argc, char **argv);

/* Free all the information stored about the actual configuration,
 * this will return the database to its initial state
 */
void ddcfg_free();

/* This function will print in the standard output, or any other file,
 * all the pairs found in the dictionary.
 */
void ddcfg_dump(const char *header, FILE *fout);

/* This function will return a pointer to a string of the specified
 * section and options. No parsing is done.
 */
char *ddcfg_get(const char *section, const char *option);

char** ddcfg_getlist(const char *section, const char *option, int *length);

/* This functions will return a parsed value for the specified section
 * and options.
 */
int ddcfg_int(const char *section, const char *option);
int ddcfg_bool(const char *section, const char *option);
double ddcfg_double(const char *section, const char *option);

/* This function will check the configuration of the database with a spec
 */
int ddcfg_check(const char *specfile);

#endif
