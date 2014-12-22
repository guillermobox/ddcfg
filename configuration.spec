#
# Specification file, kconfig-like
#
# You can use this fields:
#
# for the options:
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

OPTION Machine.endianness
	TYPE string
	DEFAULT little
	VALUES little, big
	DESCRIPTION Endianness of the input file

OPTION Machine.bits
	TYPE int
	DEPENDS_ON Machine.changebits
	DESCRIPTION Bit size of the system to simulate
	the behaviour of the system if you have changed the
	bit type of the processor. You can use 16 or 32.

OPTION Machine.changebits
	TYPE bool
	DEFAULT false
	DESCRIPTION Activate this to change the device bits

