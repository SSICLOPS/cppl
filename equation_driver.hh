// Copyright 2015-2018 RWTH Aachen University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

