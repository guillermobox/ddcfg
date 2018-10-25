SECTION Section
DESCRIPTION This is a description

PROPERTY option
TYPE real
DEFAULT thisisnotadouble

PROPERTY x
DEPENDS_ON Section.option

PROPERTY z
DEPENDS_ON This.doesnot

PROPERTY a
TYPE boolean
VALUES 1, 3, yes

PROPERTY b
TYPE boolean
DEPENDS_ON Section.b

PROPERTY c
DEFAULT x
VALUES a,b,c

PROPERTY d
DEFAULT x
VALUES a,  x    , c

PROPERTY e
VALUES a, ,d

PROPERTY f
VALUES a,

PROPERTY g
VALUES ,d

PROPERTY h
VALUES a,,d

PROPERTY sub1
POINTS_TO Doesnotexist

PROPERTY sub2
POINTS_TO Section

PROPERTY sub3
POINTS_TO Another

SECTION Another
DESCRIPTION Another section, this is empty

SUBSECTION ThisIsAValidSubsection
DESCRIPTION Yes it is
