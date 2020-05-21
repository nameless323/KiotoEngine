parser grammar ShaderInputsParser;

options { tokenVocab=ShaderInputsLexer; }

inputFile : (struct
            | cbuffer
            | tex2d
            | sampler)*;

struct : STRUCT_KEYWORD NAME CURL_BR_O variable+ CURL_BR_C SEMI;

cbuffer : CBUFFER_KEYWORD NAME CURL_BR_O variable+ CURL_BR_C SEMI;

tex2d : TEX2D_KEYWORD NAME SEMI;

sampler : SAMPLER_KEYWORD NAME SEMI;

variable : TYPE NAME SEMI;