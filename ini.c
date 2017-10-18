/*
 * Heavily modified ini parser.
 *
 * Original disclaimer:
-----------------8<---------------
 * inih -- simple .INI file parser
 *
 * inih is released under the New BSD license (see LICENSE.txt). Go to the project
 * home page for more info:
 *
 * http://code.google.com/p/inih/
-----------------8<---------------
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "ini.h"

/* Strip whitespace chars off end of given string, in place. Return s. */
static char *rstrip(char *s)
{
	char *p = s + strlen(s);
	while (p > s && isspace((unsigned char) (*--p)))
		*p = '\0';
	return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char *lskip(const char *s)
{
	while (*s && isspace((unsigned char) (*s)))
		s++;
	return (char *) s;
}

/* Return a pointer to the first comment or given char in the string. */
static char *find_char_or_comment(const char *s, char c)
{
	int was_whitespace = 0;
	while (*s && *s != c && !(was_whitespace && *s == '#')) {
		was_whitespace = isspace((unsigned char) (*s));
		s++;
	}
	return (char *) s;
}

__DDCFG_EXPORT__ int ini_parse_file(FILE * file, int (*handler) (const char *, const char *, const char *))
{
	char *line = NULL;
	size_t linelen = 0;

	char * section;
	char *start, *end, *name, *value;
	int lineno = 0, error = 0;
	size_t namelen;

	while (getline(&line, &linelen, file) != -1) {
		lineno++;
		start = line;

		start = lskip(rstrip(start));
		if (*start == '[') {

			/* A "[section]" line */
			end = find_char_or_comment(start + 1, ']');
			if (*end == ']') {
				*end = '\0';
				namelen = (size_t) (end - start);
				section = calloc(namelen + 1, sizeof(char));
				strncpy(section, start + 1, namelen);
			}

			else if (!error) {
				/* No ']' found on section line */
				error = lineno;
			}
		}

		else if (*start && *start != '#') {

			/* Not a comment, must be a name[=]value pair */
			end = find_char_or_comment(start, '=');
			if (*end == '=') {
				*end = '\0';
				name = rstrip(start);
				value = lskip(end + 1);
				end = find_char_or_comment(value, '\0');
				if (*end == '#')
					*end = '\0';
				rstrip(value);

				if (handler(section, name, value) && !error)
					error = lineno;
			}

			else if (!error) {
				error = lineno;
			}
		}
	}

	if (line)
		free(line);

	return error;
}
