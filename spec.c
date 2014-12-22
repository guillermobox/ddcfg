#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct st_spec {
	struct st_spec_entry * entries;
	unsigned int length;
};


enum entry_type {
	STRING = 0,
	INT,
	BOOL,
	DOUBLE,
	SECTION,
	SECTIONS
};

struct st_spec_entry {
	struct st_spec_entry * next;
	char * name;
	char * description;
	char * depends_on;
	char * values;
	char * defaultvalue;
	enum entry_type type;
};

char * entry_strings[] = {
	"string",
	"int",
	"bool",
	"double",
	"section",
	"sections"
};

enum parse_status {
	NOTHING,
	OPTION,
	DESCRIPTION
};

#define BUFSIZE 256

void show_entry(struct st_spec_entry * entry)
{
	printf("OPTION %s\n", entry->name);
	printf("\tDESCRIPTION %s\n", entry->description);
	if (entry->type >= 0)
		printf("\tTYPE %s\n", entry_strings[entry->type]);
	if (entry->defaultvalue)
		printf("\tDEFAULT %s\n", entry->defaultvalue);
	if (entry->depends_on)
		printf("\tDEPENDS_ON %s\n", entry->depends_on);
	if (entry->values)
		printf("\tVALUES %s\n", entry->values);
	printf("\n");
};

int setkeyvalue(struct st_spec_entry *entry, char *key, char *value)
{
	if (strcmp(key, "NAME") == 0) {
		entry->name = strdup(value);
	} else if (strcmp(key, "DESCRIPTION") == 0) {
		entry->description = strdup(value);
	} else if (strcmp(key, "DEPENDS_ON") == 0) {
		entry->depends_on = strdup(value);
	} else if (strcmp(key, "TYPE") == 0) {
		int i;
		for (i=0; i < sizeof(entry_strings) / sizeof(char*); i++) {
			if (strcmp(value, entry_strings[i]) == 0) {
				entry->type = i;
			}
		};
	} else if (strcmp(key, "DEFAULT") == 0) {
		entry->defaultvalue = strdup(value);
	} else if (strcmp(key, "VALUES") == 0) {
		entry->values = strdup(value);
	} else {
		printf("Wop, key not found! <%s>\n", key);
	}
};

int adddescription(struct st_spec_entry *entry, char *description)
{
	entry->description = realloc(entry->description, strlen(entry->description) + strlen(description) + 1);
	strcat(entry->description, " ");
	strcat(entry->description, description);
};

int main () {
	char buffer[BUFSIZE];
	char *p, *endl;
	char *value, *key;
	enum parse_status pstatus = NOTHING;
	struct st_spec_entry entry;

	memset(&entry, 0, sizeof(entry));

	struct st_spec_entry entry_list[256];
	int entry_index = 0;

	FILE *f = fopen("configuration.spec", "r");

	if (f == NULL) {
		fprintf(stderr, "Impossible to open file\n");
		return(1);
	}

	do {
		p = fgets(buffer, BUFSIZE, f);
		if (p == NULL)
			break;
		while (*p && isblank(*p))
			p++;
		if (*p == '#')
			continue;
		switch (pstatus) {
			case NOTHING:
				if (*p == '\n')
					continue;
				pstatus = OPTION;
				key = strtok(p, " ");
				value = strtok(NULL, "\n");
				setkeyvalue(&entry, "NAME", value);
				break;
			case OPTION:
				if (*p == '\n') {
					entry_list[entry_index] = entry;
					entry_index++;
					memset(&entry, 0, sizeof(entry));
					pstatus = NOTHING;
					continue;
				}
				key = p;
				key = strtok(p, " ");
				value = strtok(NULL, "\n");
				setkeyvalue(&entry, key, value);
				if (strcmp(key, "DESCRIPTION") == 0)
					pstatus = DESCRIPTION;
				break;
			case DESCRIPTION:
				if (*p == '\n') {
					entry_list[entry_index] = entry;
					entry_index++;
					memset(&entry, 0, sizeof(entry));
					pstatus = NOTHING;
					continue;
				}
				p = strtok(p, "\n");
				adddescription(&entry, p);
				break;
		};
	} while (1);

	while (entry_index--) {
		show_entry(&entry_list[entry_index]);
	}

	fclose(f);
	return 0;
};
