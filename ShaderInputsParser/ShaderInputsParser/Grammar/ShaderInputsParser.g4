parser grammar ShaderInputsParser;

options { tokenVocab=ShaderInputsLexer; }

inputFile : ( include
            | struct
            | cbuffer
            | tex2d
            | sampler)*;

include: INCLUDE_KEYWORD QUOTE FILEPATH QUOTE SEMI;

struct : STRUCT_KEYWORD NAME annotationList CURL_BR_O variable+ CURL_BR_C SEMI;

cbuffer : CBUFFER_KEYWORD NAME annotationList CURL_BR_O variable+ CURL_BR_C SEMI;

tex2d : TEX2D_KEYWORD NAME SEMI;

sampler : SAMPLER_KEYWORD NAME SEMI;

variable : TYPE NAME SEMI;

annotationList : annotation*;

annotation : PERCENT (annotationMember COMMA)* annotationMember PERCENT;

annotationMember : NAME (EQUALS VALUE)*;