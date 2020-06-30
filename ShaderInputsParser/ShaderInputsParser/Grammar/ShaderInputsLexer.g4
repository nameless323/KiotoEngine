lexer grammar ShaderInputsLexer;

fragment LETTER_NUMBER : LETTER | [0-9];
fragment LETTER : [a-zA-Z] | '_';
fragment CARET_SYMBOLS : [\n\r];

TYPE: SCALAR_TYPE | VECTOR_TYPE | MATRIX_TYPE;
MATRIX_TYPE : SCALAR_TYPE ('3x3' | '4x4')?;
VECTOR_TYPE : SCALAR_TYPE ('2' | '3' | '4')?;
SCALAR_TYPE : 'int' | 'uint' | 'float';

CURL_BR_O : '{';
CURL_BR_C : '}';
TRIG_BR_O : '<';
TRIG_BR_C : '>';
SQR_BR_O : '[';
SQR_BR_C : ']';
QUOTE : '"';
STRUCT_KEYWORD : 'struct';
CBUFFER_KEYWORD : 'constantBuffer';
SAMPLER_KEYWORD : 'sampler';
TEX2D_KEYWORD : 'texture2D';
INCLUDE_KEYWORD : '#include';
VERTEX_LAYOUT_KEYWORD : 'vertexLayout';
SHADER_KEYWORD : 'shader';

SHADER_TYPE : 'VS' | 'PS' | 'CS' | 'HS' | 'DS';
V_SEMANTIC : 'POSITION' | 'NORMAL' | 'TEXCOORD' [0-9]? | 'COLOR' [0-9]?;

NAME : LETTER+ LETTER_NUMBER*;
NUMBER : [0-9];
SEMI : ';';
COLON : ':';
FILEPATH : ('\\' | LETTER_NUMBER)+ '.' LETTER_NUMBER+;
PERCENT : '%';
EQUALS : '=';
COMMA : ',';

MULTILINE_COMMENT : '/*' .*? '*/' -> skip;
LINE_COMMENT : '//'.*? CARET_SYMBOLS -> skip;
WHITESPACE : ' ' -> skip;
TAB : [\t] -> skip;
CARET : CARET_SYMBOLS -> skip;
