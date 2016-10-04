%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.2"
%defines
%define parser_class_name {EquationParser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include <string>
#include "ast.hh"
class EquationDriver;
}

//The parsing context.
%param { EquationDriver& driver }

%locations
%initial-action
{
    //Initialize the initial location.
    @$.begin.filename = @$.end.filename = &driver.file;
};

%define parse.trace
%define parse.error verbose

%code
{
#include "equation_driver.hh"
}

%define api.token.prefix {TOK_}
%token
    END  0   "end of file"
    OR       "|"
    AND      "&"
    NOT      "!"
    EQ       "="
    NEQ      "!="
    LESS     "<"
    LEQ      "<="
    GREATER  ">"
    GEQ      ">="
    LPAREN   "("
    RPAREN   ")"
;

%token <std::string> ID "id"
%token <std::string> STRING "string"
%token <std::string> FUNCTION "function"
%token <int64_t> INTEGER "int"
%token <double> FLOAT "float"
%token <bool> BOOLEAN "bool"
%type  <AstVisitableNode *> expr term factor relation var

%printer { yyoutput << "AST Node(" << $$ << ")"; } <AstVisitableNode *>;
%printer { yyoutput << $$; } <*>;

%%

%start policy;
policy: expr END { driver.ast = new Ast($1); }
      | END { cout << "Empty Policy!" << endl; } /* blank line or a comment */
      ;
expr: term { $$ = $1; }
    | expr "|" term { $$ = new AstOperation(AstOperationType::OR, $1, $3); }
    ;
term: factor  { $$ = $1; }
    | term "&" factor { $$ = new AstOperation(AstOperationType::AND, $1, $3); }
    ;
factor: var { $$ = $1; }
      | "(" expr ")" { $$ = $2; }
      | "!" "(" expr ")" { $$ = new AstOperation(AstOperationType::NOT, $3); }
      | relation { $$ = $1; }
      ;
relation: var "="   var { $$ = new AstOperation(AstOperationType::EQUAL,   $1, $3); }
        | var "!="  var { $$ = new AstOperation(AstOperationType::NEQ,     $1, $3); }
        | var  "<"  var { $$ = new AstOperation(AstOperationType::LESS,    $1, $3); }
        | var  "<=" var { $$ = new AstOperation(AstOperationType::LEQ,     $1, $3); }
        | var  ">"  var { $$ = new AstOperation(AstOperationType::GREATER, $1, $3); }
        | var  ">=" var { $$ = new AstOperation(AstOperationType::GEQ,     $1, $3); }
        ;
var: "!" var { $$ = new AstOperation(AstOperationType::NOT, $2); }
   | BOOLEAN { AstValue value; value = $1; $$ = new AstConstant(AstValueType::Boolean, value); }
   | INTEGER { AstValue value; value = $1; $$ = new AstConstant(AstValueType::Integer, value); }
   | FLOAT   { AstValue value; value = $1; $$ = new AstConstant(AstValueType::Float,     value); }
   | STRING  { AstValue value; value = $1; $$ = new AstConstant(AstValueType::String,  value); }
   | ID { $$ = new AstId(AstValueType::Unknown, $1); }
   | FUNCTION { $$ = new AstFunction($1); }
   ;

%%

//forward error to the driver
void yy::EquationParser::error(const location_type& l, const std::string& m)  {
    driver.error(l, m);
}
