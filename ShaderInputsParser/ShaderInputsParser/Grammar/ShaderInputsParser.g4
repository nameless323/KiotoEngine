parser grammar ShaderInputsParser;

options { tokenVocab=ShaderInputsLexer; }

inputFile : ( include
            | struct
            | cbuffer
            | tex2d
            | sampler
            | vertexLayout
            | shadersBinding)*;

include: INCLUDE_KEYWORD QUOTE FILEPATH QUOTE;

struct : STRUCT_KEYWORD NAME annotationList CURL_BR_O variable+ CURL_BR_C SEMI;

cbuffer : CBUFFER_KEYWORD NAME annotationList CURL_BR_O variable+ CURL_BR_C SEMI;

tex2d : TEX2D_KEYWORD NAME SEMI;

sampler : SAMPLER_KEYWORD NAME SEMI;

variable : TYPE NAME SEMI;

annotationList : annotation*;

annotation : PERCENT (annotationMember COMMA)* annotationMember PERCENT;

annotationMember : NAME (EQUALS VALUE)?;

vertexLayout : VERTEX_LAYOUT_KEYWORD NAME CURL_BR_O vertexLayoutMember+ CURL_BR_C SEMI;

vertexLayoutMember : TYPE NAME COLON V_SEMANTIC SEMI;

shadersBinding : SHADER_KEYWORD COLON (shaderBind COMMA)* shaderBind SEMI;

shaderBind : SHADER_TYPE NAME;