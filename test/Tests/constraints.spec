SECTION A
    DESCRIPTION Doesnt matter, I just want all constraints to activate!
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
    CONDITION A.x == 1

WARNING
    DESCRIPTION Composited test
    CONDITION A.x == 1 AND A.y == 2 AND A.z == 3

WARNING
    DESCRIPTION Multiplication and addition have a particular order
    CONDITION A.x + A.y * A.z == 7

WARNING
    DESCRIPTION Check parenthesis with arithmetic operations
    CONDITION (A.x + A.y) * (A.x + A.z) == 12

WARNING
    DESCRIPTION Check also unitary negative
    CONDITION -A.x < 0

WARNING
    DESCRIPTION Integers should cast to reals
    CONDITION A.x + 1.0 > 1.9999 AND A.x + 1.0 < 2.0001

WARNING
    DESCRIPTION This is true
    CONDITION A.q1

WARNING
    DESCRIPTION This is not false
    CONDITION NOT A.q2

WARNING
    DESCRIPTION A combination of bools
    CONDITION A.q1 AND (A.q2 OR true)

WARNING
    DESCRIPTION Xor is useful to have one of two keys, but only one
    CONDITION A.q1 XOR A.q2

WARNING
    DESCRIPTION Another way of doing the previous, that scalates to 3 or more
    CONDITION A.q1
WARNING
    DESCRIPTION Combination of bools and also integer
    CONDITION (A.x + A.z < 2) OR A.q1

WARNING
    DESCRIPTION Combinations of different types, no variables here
    CONDITION 1 > 3 OR 1.0 > 3 OR yes

WARNING
    DESCRIPTION Another combination of different types
    CONDITION (3 > 0 AND -2.0 > 0.0) OR (yes AND 5 > 2.0)
