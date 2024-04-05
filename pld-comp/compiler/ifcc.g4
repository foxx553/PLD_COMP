grammar ifcc;

axiom : prog EOF;

prog : function*;
function : 'int' IDENTIFIER '(' ('int' IDENTIFIER (',' 'int' IDENTIFIER)* )? ')' block;

instruction : (return_stmt | declare_stmt | assign_stmt | call_stmt | loop | condition)+;

block : '{' instruction '}';
function_call : IDENTIFIER '(' (expression (',' expression)* )? ')';
assignation : lvalue '=' expression;
declaration : IDENTIFIER ('=' expression | '[' CONST ']')?; 
loop : 'while' '(' expression ')' block;
condition : 'if' '(' expression ')' block ('else if' '(' expression ')' block)* ('else' block)?;
lvalue : IDENTIFIER ('[' expression ']')? | '*'? IDENTIFIER;

expression: '(' expression ')'                          #exprPar
          | NOT expression                              #exprNot
          | expression OR expression                    #exprOr
          | expression AND expression                   #exprAnd
          | expression (EQ|NE|LT|GT|LE|GE) expression   #exprCmp
          | (SUB|ADD) expression                        #exprUnaire
          | expression (MUL|DIV) expression             #exprMultDiv
          | expression (ADD|SUB) expression             #exprAddSub
          | IDENTIFIER                                  #exprVariable
          | CONST                                       #exprConstante
          | function_call                               #exprFunction
          ;

return_stmt : RETURN expression ';';
declare_stmt : 'int' declaration (',' declaration)* ';';
assign_stmt : assignation ';';
call_stmt : function_call ';';

ADD : '+';
SUB: '-';
MUL : '*';
DIV : '/';
OR : '||';
AND : '&&';
NOT : '!';
EQ : '==';
NE : '!=';
LT : '<';
GT : '>';
LE : '<=';
GE : '>=';
RETURN : 'return';
CONST : [0-9]+;
COMMENT : ('/*' .*? '*/' | '//' .*? '\n' ) -> skip;
DIRECTIVE : '#' .*? '\n' -> skip;
WS    : [ \t\r\n] -> channel(HIDDEN);
IDENTIFIER : [a-zA-Z][a-zA-Z0-9]*;