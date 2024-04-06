grammar ifcc;

axiom : prog EOF;

prog : (function | declare_stmt)*;
function : (TYPE | 'void') IDENTIFIER '(' (TYPE declaration (',' TYPE declaration)* )? ')' block;

instruction : (return_stmt | declare_stmt | assign_stmt | call_stmt | loop | condition | block)*;

block : '{' instruction '}';
function_call : IDENTIFIER '(' (expression (',' expression)* )? ')';
assignation : lvalue '=' expression;
declaration : IDENTIFIER ('[' CONST ']' | '=' expression)?;
loop : 'while' '(' expression ')' block;
condition : 'if' '(' expression ')' block ('else if' '(' expression ')' block)* ('else' block)?;
lvalue : IDENTIFIER ('[' expression ']')?;

expression: '(' expression ')'                          #exprPar
          | NOT expression                              #exprNot
          | expression OR expression                    #exprOr
          | expression AND expression                   #exprAnd
          | expression (EQ|NE|LT|GT|LE|GE) expression   #exprCmp
          | (SUB|ADD) expression                        #exprUnaire
          | expression (MUL|DIV) expression             #exprMultDiv
          | expression (ADD|SUB) expression             #exprAddSub
          | lvalue                                      #exprLvalue
          | CONST                                       #exprConstante
          | function_call                               #exprFunction
          ;

return_stmt : RETURN expression ';';
declare_stmt : TYPE declaration (',' declaration)* ';';
assign_stmt : assignation ';';
call_stmt : function_call ';';

TYPE: 'int' | 'char';
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
IDENTIFIER : [_a-zA-Z][_a-zA-Z0-9]*;