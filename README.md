Configuration parser with spec checker
======================================

This library allows to import a configuration file into the code.  Once
imported, the configuration can be updated from the command line by using flags,
or merging more than one configuration file.

Several functions to parse the properties are provided, so the user can get a
single value parsed to the desired type.

Run make to compile the library. In the folder `test`, a very short main
function is written to test the library using two configuration files
and a specification file. Run make test in order to run the test.

Configuration file
------------------

The configuration file has a very simple format. It's separated in sections
and properties. Each section is shown between brakets, and each property
is defined by using an equal sign on each line, like this:

    [section]
    property = value
    otherprop = other value # with a comment that is ignored

All the whitespaces are removed, and comments are allowed starting with #.  If
several properties or sections are found, they are overwritten silently. No
property is allowed outside a section.


Specification file
------------------

A file to define the configuration can be also provided. This is the
specification file, and has a simple language that allows the user to define
which properties needs in the configuration database, which values, variable
type, or optional properties that depend on other keys. Once the specification
file is read, and the configuration database constructed, the formar can be used
to check the integrity and validate the database. Several possible errors
can be found:

  - A property is required, but not found
  - A property is found, but the content is not valid
  - A property is found that is not defined in the spec

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

In that specification, a property Machine.endianness is defined to have only two
possible values. Also, the variable Machine.bits has to be an integer, and
is only required if the variable Machine.changebits is set to true. The default
behaviour is to be false, so none of those properties are needed.

