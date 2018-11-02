SECTION A
    DESCRIPTION None of the following will fail
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
    ASSERT A.x == 1

WARNING
    DESCRIPTION Another basic test, but this one is a rejection, not assertion
    REJECT A.x < 0

WARNING
    DESCRIPTION Composited test
    ASSERT A.x == 1 AND A.y == 2 AND A.z == 3

WARNING
    DESCRIPTION Multiplication and addition have a particular order
    ASSERT A.x + A.y * A.z == 7

WARNING
    DESCRIPTION Check parenthesis with arithmetic operations
    ASSERT (A.x + A.y) * (A.x + A.z) == 12

WARNING
    DESCRIPTION Check also unitary negative
    ASSERT -A.x < 0

WARNING
    DESCRIPTION Integers should cast to reals
    ASSERT A.x + 1.0 > 1.9999 AND A.x + 1.0 < 2.0001

WARNING
    DESCRIPTION This is true
    ASSERT A.q1

WARNING
    DESCRIPTION This is not false
    ASSERT NOT A.q2

WARNING
    DESCRIPTION A combination of bools
    ASSERT A.q1 AND (A.q2 OR true)

WARNING
    DESCRIPTION Xor is useful to have one of two keys, but only one
    ASSERT A.q1 XOR A.q2

WARNING
    DESCRIPTION Combination of bools and also integer
    ASSERT (A.x + A.z < 2) OR A.q1

WARNING
    DESCRIPTION Combinations of different types, no variables here
    ASSERT 1 > 3 OR 1.0 > 3 OR yes

WARNING
    DESCRIPTION Another combination of different types
    ASSERT (3 > 0 AND -2.0 > 0.0) OR (yes AND 5 > 2.0)

WARNING
    DESCRIPTION Nested parenthesis
    ASSERT (1 + (3 + 5) * 3) * 4 == 100

WARNING
    DESCRIPTION This should warn, but it wont because it depends on a key which is false,
    DESCRIPTION so it won't be checked, but skipped.
    DEPENDS_ON A.q2
    ASSERT 0 == 1
