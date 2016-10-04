%{ /* -*- C++ -*- */
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>
#include "equation_driver.hh"
#include "equation_parser.tab.hh"

//The location of the current token.
static yy::location loc;
%}

%option noyywrap nounput batch debug noinput

blank      [ \t]+
string     \"[^\n"]+\"
idPart     [[:alpha:]][[:alnum:]]*
id         {idPart}(.{idPart}+)*
integer    0|[+-]?[1-9][0-9]*
float      [+-]?[0-9]+.[0-9]+
boolean    true|false
function   {id}\([^\)]*\)

%{
//Code run each time a pattern is matched.
#define YY_USER_ACTION  loc.columns(yyleng);
%}

%%

%{
    //Code run each time yylex is called.
    loc.step();
%}

{blank}+ { loc.step(); } /* ignore white spaces but use them for better error messages */
[\n]+    { loc.lines(yyleng); loc.step(); } /* newlines but use them for better error messages */

{function} {
    string value = string(yytext);
    return yy::EquationParser::make_FUNCTION(value, loc);
}

"&&" |
"&"  { return yy::EquationParser::make_AND(loc); }
"||" |
"|"  { return yy::EquationParser::make_OR(loc); }
"!"  { return yy::EquationParser::make_NOT(loc); }
"==" |
"="  { return yy::EquationParser::make_EQ(loc); }
"!=" { return yy::EquationParser::make_NEQ(loc); }
"<"  { return yy::EquationParser::make_LESS(loc); }
"<=" { return yy::EquationParser::make_LEQ(loc); }
">"  { return yy::EquationParser::make_GREATER(loc); }
">=" { return yy::EquationParser::make_GEQ(loc); }
"("  { return yy::EquationParser::make_LPAREN(loc); }
")"  { return yy::EquationParser::make_RPAREN(loc); }

{string} {
    string value = string(yytext);
    //remove the quotes from the value
    value = value.substr(1, value.size() - 2);
    return yy::EquationParser::make_STRING(value, loc);
}
{boolean} {
    bool value;
    if(string(yytext) == "true") {
        value = true; 
    } else {
        value = false; 
    }
    return yy::EquationParser::make_BOOLEAN(value, loc); 
}
{integer} {
    errno = 0;
    int n = strtol(yytext, NULL, 10);
    if(!(INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
        driver.error(loc, "integer is out of range");
    return yy::EquationParser::make_INTEGER(n, loc);
}
{float} {
    errno = 0;
    double f = atof(yytext);
    if(errno == ERANGE)
        driver.error(loc, "could not be converted to double");
    return yy::EquationParser::make_FLOAT(f, loc);
}
{id} { return yy::EquationParser::make_ID(yytext, loc); }

.        { driver.error (loc, "invalid character"); }
<<EOF>>  { return yy::EquationParser::make_END(loc); } 

%%

void EquationDriver::scan_begin ()  {
    yy_flex_debug = trace_scanning;
    if (file.empty () || file == "-")
        yyin = stdin;
    else if (!(yyin = fopen (file.c_str (), "r")))
    {
        error ("cannot open " + file + ": " + strerror(errno));
        exit (EXIT_FAILURE);
    }
}

void EquationDriver::scan_end ()  {
    fclose (yyin);
}

