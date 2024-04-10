grammar ifcc;

axiom : prog EOF;

prog : global*;

global : 
    return_type IDENTIFIER '(' (TYPE declaration (',' TYPE declaration)* )? ')' block     # function_global
    | TYPE declaration (',' declaration)* ';'                                                  # declaration_global
    ;

instruction : 
    'return' expression ';'                                                                     # return_stmt
    | TYPE declaration (',' declaration)* ';'                                              # declaration_stmt
    | assignment ';'                                                                            # assignment_stmt
    | function_call ';'                                                                         # function_call_stmt
    | 'break' ';'                                                                               # break_stmt
    | 'continue' ';'                                                                            # continue_stmt
    | 'while' '(' expression ')' block                                                          # loop_stmt
    | 'if' '(' expression ')' block ('else' 'if' '(' expression ')' block)* ('else' block)?     # condition_stmt
    | block                                                                                     # block_stmt
    ;

block : '{' instruction* '}';
assignment : lvalue '=' expression;
declaration : MUL? IDENTIFIER (array_length | '=' expression)?;
function_call : IDENTIFIER '(' (expression (',' expression)* )? ')';
lvalue : MUL? IDENTIFIER array_index?;
return_type : TYPE | 'void';

array_length : '[' constant ']';
array_index : '[' expression ']';
constant : NUMERIC | LITERAL;

expression: '(' expression ')'                          #exprPar
          | constant                                    #exprConstante
          | REF? lvalue                                 #exprLvalue
          | function_call                               #exprFunction
          | expression (BXOR|BOR|BAND) expression       #exprBitwiseOperation
          | (SUB|ADD) expression                        #exprUnaire
          | NOT expression                              #exprNot
          | expression (MUL|DIV|MOD) expression         #exprProduit
          | expression (ADD|SUB) expression             #exprAddSub
          | expression (LT|GT|LE|GE) expression         #exprCmp
          | expression (EQ|NE) expression               #exprCmp
          | expression AND expression                   #exprAnd
          | expression OR expression                    #exprOr
          | assignment                                  #exprAssignment
          ;

TYPE : 'int' | 'char';
REF : '&';
ADD : '+';
SUB: '-';
MUL : '*';
DIV : '/';
MOD: '%';
OR : '||';
AND : '&&';
BAND : '& ';
BXOR : '^';
BOR : '|';
NOT : '!';
EQ : '==';
NE : '!=';
LT : '<';
GT : '>';
LE : '<=';
GE : '>=';
NUMERIC : [0-9]+ ;
LITERAL : '\'' '\\'? . '\'';
COMMENT : ('/*' .*? '*/' | '//' .*? '\n' ) -> skip;
DIRECTIVE : '#' .*? '\n' -> skip;
WS : [ \t\r\n] -> channel(HIDDEN);
IDENTIFIER : [_a-zA-Z][_a-zA-Z0-9]*;