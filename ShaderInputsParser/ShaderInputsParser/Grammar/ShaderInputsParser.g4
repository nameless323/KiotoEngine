parser grammar ShaderInputsParser;

options { tokenVocab=ShaderInputsLexer; }

inputFile : (struct | variable)*;

struct : STRUCT_KEY NAME CURL_BR_O variable+ CURL_BR_C SEMI;

variable : TYPE NAME SEMI;