## AIM

To design and implement a simple compiler using LEX and YACC to perform lexical analysis, syntax analysis, three-address code generation, code optimization using code motion, and target code generation.

## OVERALL ALGORITHM

1. Start.
2. Read the source program as input.
3. Perform lexical analysis using LEX to identify keywords, identifiers, constants, operators, and special symbols.
4. Pass the identified tokens to YACC for syntax analysis.
5. Validate the source program using the grammar rules defined in YACC.
6. Generate intermediate code in the form of three-address code using syntax-directed translation.
7. Optimize the intermediate code by identifying loop-invariant expressions and moving eligible computations outside loops.
8. Generate target code from the optimized intermediate representation.
9. Display the lexical analysis output, syntax analysis result, three-address code, optimized code, and target code.
10. Verify the output of each phase to ensure correct compilation.
11. Stop.

## SPECIFICATION OF LEX AND YACC

### 1. LEX Specification

LEX is a lexical analyzer generator that converts input characters into tokens using regular expressions.

Main components:

- Definitions: Declare header files, variables, and regular expression definitions.
- Rules: Specify patterns and corresponding actions for recognizing tokens.
- User subroutines: Contain supporting C functions such as `main()` and error-handling functions.

Functions:

- Identifies keywords, identifiers, constants, operators, and symbols.
- Ignores whitespace and comments.
- Returns tokens to YACC for syntax analysis.

### 2. YACC Specification

YACC (Yet Another Compiler Compiler) is a parser generator that uses grammar rules to check the syntax of a source program.

Main components:

- Declarations: Define tokens, data types, and variables.
- Grammar rules: Specify syntax rules and semantic actions.
- User subroutines: Contain supporting C functions such as `main()` and `yyerror()`.

Functions:

- Receives tokens from LEX.
- Checks the syntax of assignments, arithmetic expressions, and looping statements.
- Generates three-address code using syntax-directed translation.
- Reports syntax errors when the input violates the grammar.

## BEST PRACTICES

1. Use meaningful token names and clear grammar rules.
2. Define regular expressions carefully to avoid incorrect token recognition.
3. Handle whitespace, comments, and invalid tokens properly.
4. Use temporary variables and labels consistently during intermediate code generation.
5. Apply code motion only to expressions that are loop-invariant and safe to move.
6. Maintain modularity by separating lexical analysis, parsing, optimization, and target code generation.
7. Handle syntax errors and invalid inputs without unexpected program termination.
8. Test the compiler with different valid and invalid source programs.
9. Use consistent formatting for intermediate and target code.
10. Verify the output at each compilation phase to ensure correctness.

## LEARNING OUTCOMES

After completing this experiment, students will be able to:

1. Understand the phases of a compiler and their functions.
2. Design lexical analyzers using LEX specifications and regular expressions.
3. Construct grammar rules and perform syntax analysis using YACC.
4. Generate three-address code using syntax-directed translation.
5. Apply code optimization techniques such as loop-invariant code motion.
6. Generate target instructions from optimized intermediate code.
7. Integrate compiler modules and verify the correctness of the compilation process.
