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
The library allows the user to check the validity of the configuration database,
and print an error message if there is any mismatch between the spec and the
configuration database. See section **Specification file** for more information.

Compile the library running make, you will obtain a `ddcfg.a` to statically
link into your application. Usually about 30 KB in size. For today, no shared
library version is available.

After compiling, a small test is found in the folder `test`.

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
  - **DEPENDS_ON**, this property is only needed if the named property is True
  - **POINTS_TO**, this property gives the names of the subsection it points to

An example of specification is this:

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

