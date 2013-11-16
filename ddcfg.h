/* ddcfg.h
 *
 * Header file for density-gradient configuration parser
 */

/* Parse a file to the dictionary of key/value pairs. Call this
 * before any other call, otherwise they will return error.
 */
int ddcfg_parse(char *filename);

/* Parse the arguments from the command line, call it after cfg_parse to
 * mantain the priority of the command line arguments over the configuration
 * file
 */
int ddcfg_parse_args(int argc, char **argv);

/* This function will return a pointer to a string of the specified
 * section and options. No parsing is done.
 */
char *ddcfg_get(const char *section, const char *option);

/* This functions will return a parsed value for the specified section
 * and options.
 */
int ddcfg_int(const char *section, const char *option);
int ddcfg_bool(const char *section, const char *option);
double ddcfg_double(const char *section, const char *option);
