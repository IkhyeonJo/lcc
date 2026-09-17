# LCC 3.6 — C89/C90 Used Syntax Tests

목적:
- LCC 3.6 자체 소스(`src/`, `cpp/`, `lburg`)에서 실제 사용이 확인된 C89/C90 문법을 독립적인 입력 프로그램으로 다시 검증한다.
- `LCC_3.6_C89_C90_Unused_Syntax_Tests_Final.zip`과 함께 사용한다.
- 이 세트는 "LCC가 구현한 모든 문법"의 형식적 완전성 증명이 아니라, 이번 소스 감사에서 실제 사용이 확인된 대표 문법을 독립 테스트하는 세트다.

권장 검증:
1. LCC로 preprocessing/compilation
2. assembly 생성
3. assembler/linker
4. 실행 결과 확인

주의:
- 실패 시 parser, semantic analysis, type system, backend, runtime/library 중 어느 단계인지 구분한다.
- target/ABI에 따라 sizeof, integer representation 등의 결과값은 달라질 수 있다.
- `#line`은 소스 감사에서 실제 사용이 확인된 전처리 지시문이므로 포함했다.
- `auto`, standalone `signed`, `volatile`, `const volatile`, abstract array type-name, tag-only struct declaration,
  `#elif`, `#error`, `#pragma`, null preprocessing directive 등은 별도의 Unused 테스트 세트에서 다룬다.
