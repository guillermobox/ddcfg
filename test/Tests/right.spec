# THIS IS A FIRST LINE COMMENT

SECTION DefaultValues
DESCRIPTION Check that default values work correctly

# ANOTHER COMMENT
PROPERTY a
TYPE double
# EVEN HERE COMMENTS ARE OK
# WITH MORE THAN ONE LINE
DEFAULT 3.141592873642829385612

PROPERTY b
TYPE bool
DEFAULT true

PROPERTY c
TYPE bool
DEFAULT YES

PROPERTY d
TYPE bool
DEFAULT No

PROPERTY e
TYPE bool
DEFAULT 0

PROPERTY f
TYPE bool
DEFAULT 1

PROPERTY g
TYPE int
DEFAULT 0

PROPERTY h
TYPE int
DEFAULT -20

PROPERTY i
TYPE int
DEFAULT 91923

SECTION ListValues
DESCRIPTION Check the facility to have lists of values

PROPERTY a
DESCRIPTION Integer from a list
TYPE int
VALUES 1,2,3

PROPERTY b
DESCRIPTION Integer from a list
TYPE int
VALUES 123,456,789

PROPERTY c
DESCRIPTION String from a list
VALUES all,these,are,ok

PROPERTY d
DESCRIPTION Double from a list
TYPE double
VALUES 1.23,9.0,0.00000123

SECTION Dependencies
DESCRIPTION Check dependencies of the keys

PROPERTY true
DESCRIPTION This is always true
DEFAULT true
TYPE bool

PROPERTY false
DESCRIPTION This is always false
DEFAULT false
TYPE bool

PROPERTY notrequired
DESCRIPTION This is not required, depends on false key
DEPENDS_ON Dependencies.false

PROPERTY required
DESCRIPTION This is required, depends on true key
DEPENDS_ON Dependencies.true

PROPERTY alsonotrequired
DESCRIPTION This is not required because it has its own default value
DEPENDS_ON Dependencies.true
DEFAULT set

PROPERTY switch1
DESCRIPTION This will be switched in the config file
TYPE bool

PROPERTY if_switch1
DESCRIPTION Depends on the previous switch
DEPENDS_ON Dependencies.switch1

PROPERTY switch2
TYPE bool

PROPERTY if_switch2
DEPENDS_ON Dependencies.switch1

SECTION Dependencies2
DESCRIPTION The same as Dependencies, but now between
DESCRIPTION different sections. To these keys point to the
DESCRIPTION previous section.

PROPERTY remote_required
DEPENDS_ON Dependencies.true

PROPERTY remote_notrequired
DEPENDS_ON Dependencies.false

SECTION Subsections
DESCRIPTION Check the subsections

PROPERTY list
DESCRIPTION This list has pointers to subsections
TYPE subsection
POINTS_TO Subtype

SUBSECTION Subtype
DESCRIPTION These sections are special, they are subsections with diferent names

PROPERTY value
DESCRIPTION A single value if ok for testing
TYPE int

