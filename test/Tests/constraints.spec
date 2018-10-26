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
