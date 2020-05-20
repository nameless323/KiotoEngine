lexer grammar ShaderInputsLexer;

TYPE: SIMPLE_TYPE SIMPLE_TYPE_MODIFIERS?;
SIMPLE_TYPE_MODIFIERS : '2' | '3' | '4' | '3x3' | '4x4';
SIMPLE_TYPE : 'int' | 'uint' | 'float';

CURL_BR_O : '{';
CURL_BR_C : '}';
STRUCT_KEY : 'struct';

NAME : LETTER+ LETTER_NUMBER*;
SEMI : ';';

fragment LETTER_NUMBER : (LETTER | [0-9])+;
fragment LETTER : [a-zA-Z] | '_';


WHITESPACE : ' ' -> skip;
TAB : [\t] -> skip;
CARRET : [\n\r] -> skip;
