SECTION A
    DESCRIPTION All the following conditions will fail
    PROPERTY x
        TYPE integer
        DEFAULT 1
    PROPERTY y
        TYPE integer
        DEFAULT 2
    PROPERTY z
        TYPE integer
        DEFAULT 3
    PROPERTY q1
        TYPE boolean
        DEFAULT yes
    PROPERTY q2
        TYPE boolean
        DEFAULT no

WARNING
    DESCRIPTION Basic test
    ASSERT A.x != 1

FAILURE
    DESCRIPTION Composited test
    ASSERT A.x + A.y > A.z

FAILURE
    DESCRIPTION This is not a boolean, even though is valued 1
    ASSERT A.x
