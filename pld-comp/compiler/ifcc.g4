grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' instruction '}' ;

instruction : (return_stmt | affect_stmt)+ ;

return_stmt : RETURN CONST ';' ;
affect_stmt : 'int' IDENTIFIER '=' (CONST | IDENTIFIER) ';' ;

RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
IDENTIFIER : [a-zA-Z][a-zA-Z0-9]* ;
