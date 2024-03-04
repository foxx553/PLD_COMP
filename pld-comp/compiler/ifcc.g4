grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' instructions '}' ;

instructions :   return_stmt
             |   instructions ';' instructions
             |   'int' VAR '=' VAR
             |   'int' VAR '=' CONST
             ;

return_stmt :    RETURN CONST ';' 
            |    RETURN VAR ';'
            ;

RETURN : 'return' ;
VAR : [a-zA-Z][a-zA-Z0-9]+ ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
