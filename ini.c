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

#define MAX_SECTION 64
#define MAX_NAME 64

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

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
static char *strncpy0(char *dest, const char *src, size_t size)
{
	strncpy(dest, src, size);
	dest[size - 1] = '\0';
	return dest;
}

__DDCFG_EXPORT__ int ini_parse_file(FILE * file, int (*handler) (const char *, const char *, const char *))
{
	char *line = NULL;
	size_t linelen = 0;

	char section[MAX_SECTION] = "";
	char prev_name[MAX_NAME] = "";
	char *start, *end, *name, *value;
	int lineno = 0, error = 0;

	/* Scan through file line by line */

	while (getline(&line, &linelen, file) != -1) {
		lineno++;
		start = line;

		start = lskip(rstrip(start));
		if (*start == '[') {

			/* A "[section]" line */
			end = find_char_or_comment(start + 1, ']');
			if (*end == ']') {
				*end = '\0';
				strncpy0(section, start + 1, sizeof(section));
				*prev_name = '\0';
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

				strncpy0(prev_name, name, sizeof(prev_name));

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
