#
# Specification file, kconfig-like
#
# You can use this fields:
#
# for the properties:
#       TYPE string|int|bool|double|section|sections
#       DEFAULT default_value
#       VALUES possible_values, comma_separated
#       DEPENDS_ON other_bool_key
#       DESCRIPTION description here
#
# for the sections:
#       DESCRIPTION description here
#

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

PROPERTY modules
	TYPE sections
	POINTS_TO Module
	DESCRIPTION List of modules to be loaded

SUBSECTION Module
	DESCRIPTION Module to be used

PROPERTY Module.name
	DESCRIPTION Name of the module, user friendly

PROPERTY Module.activate
	TYPE bool
	DESCRIPTION Activate the module when loading the program

PROPERTY Module.port
	TYPE int
	DESCRIPTION Port to acces to module once loaded
