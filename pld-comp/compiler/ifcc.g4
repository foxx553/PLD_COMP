grammar ifcc;

axiom : prog EOF ;

prog : function* ;
function : 'int' IDENTIFIER '(' ('int' IDENTIFIER (',' 'int' IDENTIFIER)* )? ')' '{' instruction '}';

instruction : (return_stmt | declare_stmt | assign_stmt | call_stmt)+ ;

function_call : IDENTIFIER (('(' ')') | ('(' expression (',' expression)* ')')) ;
assignation : IDENTIFIER '=' expression ;
declaration : IDENTIFIER | assignation ;

expression: '(' expression ')'                  #exprPar
          | (SUB|ADD) expression                #exprUnaire
          | expression (MUL|DIV) expression     #exprMultDiv
          | expression (ADD|SUB) expression     #exprAddSub
          | IDENTIFIER                          #exprVariable
          | CONST                               #exprConstante
          | function_call                       #exprFunction
          ;

return_stmt : RETURN expression ';' ;
declare_stmt : 'int' declaration (',' declaration)* ';' ;
assign_stmt : assignation ';';
call_stmt : function_call ';';

ADD : '+';
SUB: '-';
MUL : '*';
DIV : '/';
RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : ('/*' .*? '*/' | '//' .*? '\n' ) -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
IDENTIFIER : [a-zA-Z][a-zA-Z0-9]* ;