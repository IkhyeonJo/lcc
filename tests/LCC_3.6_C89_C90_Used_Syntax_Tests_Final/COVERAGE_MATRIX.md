# Used Syntax Coverage

This set is intended to re-validate, using independent inputs, the syntax confirmed to be actually used during the LCC 3.6 source audit.

| File | Main syntax |
|---|---|
| 01 | declarations, extern/static/register, const |
| 02 | char/short/int/long/float/double, signedness via unsigned, typedef-name |
| 03 | const-qualified declarations/pointers |
| 04 | pointer, address-of, dereference |
| 05 | array, indexing, pointer arithmetic |
| 06 | prototypes, typed parameters, function calls, void |
| 07 | old-style function definition |
| 08 | empty parameter list |
| 09 | ellipsis/variadic parameter list |
| 10 | function pointer, nested declarator |
| 11 | struct/union/enum |
| 12 | bit-field |
| 13 | initializer-list and trailing comma |
| 14 | unary/binary/logical/relational/bitwise/conditional operators |
| 15 | all compound assignment operators |
| 16 | comma, conditional, sizeof, pointer type-name |
| 17 | if/else, while, do-while, for, switch/case/default, continue/break |
| 18 | label, goto, return |
| 19 | include/define/undef/ifdef/ifndef/if/else/endif/line, function-like macro |
| 20 | abstract pointer/function type-names, sizeof |
| 21 | struct typedef-name, enum typedef-name, self-referential pointer |

This table is based on "representative independent tests" and is not a formal verification matrix that maps every alternative in the standard grammar to a separate file on a one-to-one basis.
