#pragma once

#include <string>
#include "equation_parser.tab.hh"
#include "ast.hh"

// Tell Flex the lexer's prototype ...
#define YY_DECL \
    yy::EquationParser::symbol_type yylex (EquationDriver& driver)
// ... and declare it for the parser's sake.
YY_DECL;

// Conducting the whole scanning and parsing of a CPPL equation
class EquationDriver  {
    public:
        EquationDriver();
        virtual ~EquationDriver();

        // Handling the scanner.
        void scan_begin ();
        void scan_end ();
        bool trace_scanning;

        //Will contain the AST result after a parse process
        Ast *ast; 

        // Run the parser on file F.
        // Return 0 on success.
        void parse (const std::string& f);
        // The name of the file being parsed.
        // Used later to pass the file name to the location tracker.
        std::string file;
        // Whether parser traces should be generated.
        bool trace_parsing;

        // Error handling.
        void error (const yy::location& l, const std::string& m);
        void error (const std::string& m);
};

