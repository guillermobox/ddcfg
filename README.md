Configuration parser with spec checker
======================================

This C library allows the user to manage a configuration database consisting on
key-value pairs which are parsed into a C string, list, bool, integer, or
double. It is designed to be used to have a global configuration available from
all the program.

The contents are loaded into the configuration database with `ini` files.  These
files have a very simple format, and are plain text human readable. See section
**Configuration file** for more information. We have implemented only a subset
of the `ini` format, in order to simplify the definition of this files. There is
also implemented a mechanism to insert pairs in the configuration database via
command line flags, overwriting the values provided by the `ini` files.

Once the configuration database is loaded into memory, the user can check a
given specification (spec). A spec is also a plain text file, that explains in a
verbose way some requirements that the configuration database has to comply to.
The library allows the user to check the validity of the database, and print an
error message if there is any mismatch with the spec. See section
**Specification file** for more information.

Compile the library running make, you will obtain a `ddcfg.a` to statically
link into your application. Usually about 30 KB in size. For today, no shared
library version is available.

After compiling, a small test is found in the folder `test`.


Usage
-----

From the source code, just import the library:

    #include "ddcfg.h"

And then, you have `ddcfg_parse` to load a new `ini` file into the db:

    ddcfg_parse("file.ini");

From this point on, the configuration database has content and can be used
globally from any point from the source. In order to get a pointer to the
value of a given key-pair value:

    char * p = ddcfg_get("section", "property");

This will fail and exit the program if the pair is not defined in the database.
In order to check previously if the pair is defined, do:

    char * p = ddcfg_is_defined("section", "property");

This call will return the value, or NULL if it's not defined. In both cases, the
value is a pointer to the internal structure, so don't free the pointer p after
using the value it points to. Also, don't change it. There is also the
possibility of parsing the value directly:

    int number = ddcfg_int("section", "property");
    double realnumber = ddcfg_double("section", "property");
    if (ddcfg_bool("section", "property"))
    char * listvalues = ddcfg_getlist("section", "property", &size);

The parsing works as follows: if it success, return the desired value. If the
parsing fails, it stops the execution of the program and prints an error message.
This is a little excesive, but it's done in order to prevent execution in error
state. In the case of `ddcfg_bool`, the parser will return 1 if the pair has
value yes, on, or true; and it will return 0 if the pair has value no, off, or
false. Any combination of big and small case is allowed.

To manage the spec file, some function calls are also provided. To load the
specfile from a file or buffer, use: `ddcfg_load_specfile` or
`ddcfg_load_specdata`. After the spec is loaded, use `ddcfg_check_spec` to
check it against the loaded database. If there is an error, a positive number
indicating the number of errors is returned. Errors are printed in the
standard error.

In any moment, both the database and the loaded spec can be dumped back to the
terminal, by using `ddcfg_dump` and `ddcfg_dump_spec`. These functions will
print a working version of the database, in ini format, and a working version
of the specification, in our spec format. So, they can be explored, changed,
and reloaded in the code.

After you have finished with everything, `ddcfg_free` will free all the
resources.

To see a working example, look the spec checker at `test/main.c`.

Configuration file
------------------

The configuration file has a very simple format. It's separated in sections
and properties. Each section is shown between brakets, and each property
is defined by using an equal sign between the key and the value, like this:

    [section]
    property = value
    otherprop = other value

All the whitespaces are removed at the start of the line, at both sides of the
equal sign, and at the end of he line. Also, empty lines in any point of the
file are welcome. Comments are allowed starting with #, alone in a single line
or inline at any point. If several properties or sections are found, they are
overwritten silently so the last pair found is priorized. No property is allowed
outside a section.

Internally, and in the command line, the key-value pairs have a slightly
different notation. Just concatenate the section name, and the property name
with a dot, like this: `section.property`. This format is used in error
handling, and when changing the configuration database from the command line.
In order to do this, the developer has to call `ddcfg_parse_args`, with the
`argc` and `argv` variables. This library will search for the flag `--config`,
and then take the next two arguments as the key and the value desired. For
example, if the executable is called `program`:

    $ program --config HTTP.security ssl

That will change the value of the property `security` from the section `HTTP`
to have the value `ssl`. If the property didn't exist before, it's created.

Specification file
------------------

The specification file has a verbose description of the requirements to be
desired over the configuration file. The user can write a spec file to test the
configuration database for completeness before launching the application. It
also can be used to provide default values to certain pairs in order to maintain
backwards compatibility or improve the robustness of the application.

Once the specification file is read, and the configuration database constructed,
the former can be used to check the integrity and validate the database. Several
possible errors can be found:

  - A property is required, but not found
  - A property is found, but the content is not parseable to the desired type
  - A property is found, but the content is not in the allowed value range
  - A property is found that is not defined in the spec

The specification file consist on lines, each one with a first keyword and a
content. The possible keywords are:

  - **SECTION**, indicates that the following lines refeer to this section
  - **SUBSECTION**, indicates that we are working with a subsection (look ahead)
  - **DESCRIPTION**, describes the section, or subsection, or property
  - **PROPERTY**, indicates a property for the active section
  - **TYPE**, can be `double`, `int`, `bool`, `string`, `list`, `subsection`
  - **DEFAULT**, give a default value to be used in case of absence from db
  - **VALUES**, give a list of possible values this property can have
  - **DEPENDS_ON**, this property is only needed if the named property is True
  - **POINTS_TO**, this property gives the names of the subsection it points to

Only the description line is allowed after a section and a subsection. In this
file, lines beginning with # will be considered to be comments, and ignored.
Also, any ammount of space between lines, at the start of the line, at the end
of the line, or between the line keyword and the line content is ignored.
Usually using a little indentation to differentiate the SECTION, PROPERTY, and
SUBSETION from the other lines is a very good clue to make the file more human
readable, but it's completely optional. An example of specification is this:

    SECTION Machine
    	DESCRIPTION Information about the Machine to be used with the compiler
    PROPERTY endianness
    	TYPE string
    	DEFAULT little
    	VALUES little, big
    	DESCRIPTION Endianness of the input file
    PROPERTY bits
    	TYPE int
    	DEPENDS_ON Machine.changebits
    	DESCRIPTION Bit size of the system to simulate
    	DESCRIPTION the behaviour of the system if you have changed the
    	DESCRIPTION bit type of the processor. You can use 16 or 32.
    PROPERTY changebits
    	TYPE bool
    	DEFAULT false
    	DESCRIPTION Activate this to change the device bits

In that specification, a property `Machine.endianness` is defined to have only two
possible values. Also, the variable `Machine.bits` has to be an integer, and
is only required if the variable `Machine.changebits` is set to true. The default
behaviour is to be false, so none of those properties are needed.

So, in this case, this configuration is valid:

    [Machine]
    endianness = little

The variable `Machine.changebits` will have the default value of false, and the
`Machine.bits` will not be defined. But this configuration fails:

    [Machine]
    endianness = top
    changebits = true

For two reasons: the endianness doesn't have the right value, and the changebits
variable is true, which means the `Machine.bits` property is required.

