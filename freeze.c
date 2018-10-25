
void structure_property(struct st_spec_property *prop) 
{
        if (prop != NULL) {
        const char * type;
        if (prop->type == T_TYPE_INTEGER) {
            type = "int";
        } else if (prop->type == T_TYPE_REAL) {
            type = "double";
        } else if (prop->type == T_TYPE_BOOLEAN) {
            type = "int";
        } else if (prop->type == T_TYPE_STRING) {
            type = "char* ";
        } 
        printf("        %s %s; /*!< %s */\n", type, prop->name, prop->description);
        structure_property(prop->next);
    }
}

void structure_section(struct st_spec_section *sec)
{
    printf("\nstruct ddcfg_database {\n");
    while (sec) {
        printf("    struct {\n");
        structure_property(sec->properties);
        printf("    } %s; /*!< %s */\n", sec->name, sec->description);
        sec = sec->next;
    }
    printf("} database;\n\n");
}

void load_structure_property(struct st_spec_property *prop)
{
    const char *sec, *pro, *command;
    sec = prop->section->name;
    pro = prop->name;
    switch(prop->type) {
        case T_TYPE_INTEGER: command = "ddcfg_int"; break;
        case T_TYPE_REAL:    command = "ddcfg_double"; break;
        case T_TYPE_BOOLEAN: command = "ddcfg_bool"; break;
        case T_TYPE_STRING:  command = "ddcfg_get"; break;
        default: break;
    }
    printf("    database.%s.%s = %s(\"%s\", \"%s\");\n", sec, pro, command, sec, pro);
}

void load_structure_section(struct st_spec_section *sec)
{
    struct st_spec_property *prop = sec->properties;
    while (prop) {
        load_structure_property(prop);
        prop = prop->next;
    }
}

void load_structures(struct st_spec * spec)
{
    struct st_spec_section * sec = spec->sections;

    printf("void freeze_database()\n{\n");
    while (sec) {
        load_structure_section(sec);
        sec = sec->next;
    }
    printf("}\n");
}
