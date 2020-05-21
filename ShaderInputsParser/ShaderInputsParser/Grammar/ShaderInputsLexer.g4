lexer grammar ShaderInputsLexer;

fragment LETTER_NUMBER : (LETTER | [0-9])+;
fragment LETTER : [a-zA-Z] | '_';
fragment CARET_SYMBOLS : [\n\r];

TYPE: SCALAR_TYPE VECTOR_TYPE_MODIFIERS?;
VECTOR_TYPE_MODIFIERS : '2' | '3' | '4' | '3x3' | '4x4';
SCALAR_TYPE : 'int' | 'uint' | 'float';

CURL_BR_O : '{';
CURL_BR_C : '}';
STRUCT_KEYWORD : 'struct';
CBUFFER_KEYWORD : 'constantBuffer';
SAMPLER_KEYWORD : 'sampler';
TEX2D_KEYWORD : 'texture2D';

NAME : LETTER+ LETTER_NUMBER*;
SEMI : ';';

MULTILINE_COMMENT : '/*' .*? '*/' -> skip;
LINE_COMMENT : '//'.*? CARET_SYMBOLS -> skip;
WHITESPACE : ' ' -> skip;
TAB : [\t] -> skip;
CARET : CARET_SYMBOLS -> skip;
