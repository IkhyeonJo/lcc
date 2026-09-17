# Used Syntax Coverage

이번 세트는 LCC 3.6 소스 감사에서 실제 사용이 확인된 문법을 독립 입력으로 재검증하기 위한 것이다.

| 파일 | 주요 문법 |
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

이 표는 "대표 독립 테스트" 기준이며, 표준 grammar의 모든 대안을 파일 하나씩 1:1로 매핑한 형식 검증표는 아니다.
