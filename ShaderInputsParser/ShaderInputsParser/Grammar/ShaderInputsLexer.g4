lexer grammar ShaderInputsLexer;

TYPE: SIMPLE_TYPE SIMPLE_TYPE_MODIFIERS?;
SIMPLE_TYPE_MODIFIERS : '2' | '3' | '4' | '3x3' | '4x4';
SIMPLE_TYPE : 'int' | 'uint' | 'float';

NAME : LETTER+ LETTER_NUMBER*;

fragment LETTER_NUMBER : (LETTER | [0-9])+;
fragment LETTER : [a-zA-Z] | '_';