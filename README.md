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


