grammar ifcc;

axiom : prog EOF;

prog : (function | declare_stmt)*;
function : (TYPE | VOID) IDENTIFIER '(' (TYPE declaration (',' TYPE declaration)* )? ')' block;

instruction : (return_stmt | declare_stmt | assign_stmt | call_stmt | loop | condition | block | break_stmt | continue_stmt)*;

block : '{' instruction '}';
function_call : IDENTIFIER '(' (expression (',' expression)* )? ')';
assignation : lvalue '=' expression;
declaration : IDENTIFIER ('[' constant ']' | '=' expression)?;
loop : 'while' '(' expression ')' block;
condition : 'if' '(' expression ')' block ('else if' '(' expression ')' block)* ('else' block)?;
lvalue : IDENTIFIER ('[' expression ']')?;

expression: '(' expression ')'                          #exprPar
          | constant                                    #exprConstante
          | lvalue                                      #exprLvalue
          | function_call                               #exprFunction
          | (SUB|ADD) expression                        #exprUnaire
          | NOT expression                              #exprNot
          | expression (MUL|DIV|MOD) expression         #exprProduit
          | expression (ADD|SUB) expression             #exprAddSub
          | expression (LT|GT|LE|GE) expression         #exprCmp
          | expression (EQ|NE) expression               #exprCmp
          | expression AND expression                   #exprAnd
          | expression OR expression                    #exprOr
          | assignation                                 #exprAssignment
          ;

return_stmt : RETURN expression ';';
declare_stmt : TYPE declaration (',' declaration)* ';';
assign_stmt : assignation ';';
call_stmt : function_call ';';
break_stmt : 'break' ';';
continue_stmt : 'continue' ';';
constant : NUMERIC | LITERAL ;

TYPE: 'int' | 'char';
VOID: 'void';
ADD : '+';
SUB: '-';
MUL : '*';
DIV : '/';
MOD: '%';
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
NUMERIC : [0-9]+ ;
LITERAL : '\'' '\\'? . '\'';
COMMENT : ('/*' .*? '*/' | '//' .*? '\n' ) -> skip;
DIRECTIVE : '#' .*? '\n' -> skip;
WS    : [ \t\r\n] -> channel(HIDDEN);
IDENTIFIER : [_a-zA-Z][_a-zA-Z0-9]*;