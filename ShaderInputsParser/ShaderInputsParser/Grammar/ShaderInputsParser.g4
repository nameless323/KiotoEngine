parser grammar ShaderInputsParser;

options { tokenVocab=ShaderInputsLexer; }

variable : TYPE ' ' NAME;