grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' instruction '}' ;

instruction : (return_stmt | declare_stmt | assign_stmt)+ ;

expression: '(' expression ')'                  #exprPar
          | ('-' | '+') expression              #exprUnaire
          | expression ('*'|'/') expression     #exprMultDiv
          | expression ('+'|'-') expression     #exprAddSub
          | IDENTIFIER                          #exprVariable
          | CONST                               #exprConstante
          ;

return_stmt : RETURN expression ';';
declare_stmt : 'int' (IDENTIFIER ';' | assign_stmt);
assign_stmt : IDENTIFIER '=' expression ';';

value : CONST | IDENTIFIER;

RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
IDENTIFIER : [a-zA-Z][a-zA-Z0-9]* ;
