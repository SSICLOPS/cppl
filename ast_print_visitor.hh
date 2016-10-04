#pragma once

#include "ast.hh"

//Visitor that prints the AST in a nice way
class AstPrintVisitor : public AstVisitor  {
    public:
        void visit(AstOperation &);
        void visit(AstConstant &);
        void visit(AstId &);
        void visit(AstFunction &);
        void visit(Ast &);

    private:
        uint16_t indent;
};
