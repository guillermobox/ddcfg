/* ddcfg.h
 *
 * Header file for density-gradient configuration parser
 */

#ifndef __DDCFG_H__
#define __DDCFG_H__

/* Parse a file to the dictionary of key/value pairs. Call this
 * before any other call, otherwise they will return error.
 */
int ddcfg_parse(const char *filename);

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

/* This function will return a number from 0 to n if the value assigned in
 * the dictionary matches one of the provided in the variable args. If none,
 * -1 is teturned.
 */
int ddcfg_select(const char *section, const char *option, int n, ...);

/* This function checks if a certain section.option is defined
 */
char * ddcfg_is_defined(const char *section, const char *option);

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
/* Load a spec file and parse the contents into the spec tree.
 */
int ddcfg_load_specfile(const char *specfile);

/* Load the spec contents directly from memory and parse them. The contents
 * will be copied to a internal buffer.
 */
int ddcfg_load_specdata(const char *contents, int length);

/* Check if the configuration database complies with the spec. Return 0 on
 * exit, and any number of errors if not.
 */
int ddcfg_check_spec();

/* Dump the full contents of the spec in stdout, to check the line number
 * when a spec error occurs.
 */
void ddcfg_dump_spec();

#endif
