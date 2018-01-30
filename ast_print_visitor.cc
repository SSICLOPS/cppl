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

#include "ast_print_visitor.hh"

void AstPrintVisitor::visit(AstOperation &op) {
    //indent the output if needed
    if(indent)
        cout << setw(indent) << ' ';

    //print the operator type as string
    if(op.type == AstOperationType::NOT)
        cout << "!";
    else if(op.type == AstOperationType::AND)
        cout << "&";
    else if(op.type == AstOperationType::OR)
        cout << "|";
    else if(op.type == AstOperationType::EQUAL)
        cout << "=";
    else if(op.type == AstOperationType::NEQ)
        cout << "!=";
    else if(op.type == AstOperationType::LESS)
        cout << "<";
    else if(op.type == AstOperationType::LEQ)
        cout << "<=";
    else if(op.type == AstOperationType::GREATER)
        cout << ">";
    else if(op.type == AstOperationType::GEQ)
        cout << ">=";
    else if(op.type == AstOperationType::IS_TRUE)
        cout << "=TRUE";
    else if(op.type == AstOperationType::IS_FALSE)
        cout << "=FALSE";
    else if(op.type == AstOperationType::ELIMINATED_NOT)
        cout << "Eliminated NOT";
    else
        cerr << "Unknown AST operator type in PrintVisitor!" << endl;

    //add the number of children to the operation type
    cout << "(" << (int) op.getNumberOfChildren() << ")" << endl;

    //call the print visitor also for the children of the node
    for(uint8_t i = 0; i < op.getNumberOfChildren(); i++)  {
        int oldIndent = indent;
        indent += 4;
        op.getChild(i)->accept(*this);
        indent = oldIndent;
    }
}

void AstPrintVisitor::visit(AstConstant &constant) {
    //indent the output if needed
    if(indent)
        cout << setw(indent) << ' ';

    if(constant.type == AstValueType::Integer)
        cout << "int(" << boost::get<int64_t>(constant.value) << ")" << endl;
    else if(constant.type == AstValueType::Float)
        cout << "float(" << boost::get<double>(constant.value) << ")" << endl;
    else if(constant.type == AstValueType::String)
        cout << "string(\"" << boost::get<string>(constant.value) << "\")" << endl;
    else if(constant.type == AstValueType::EnumValue)
        cout << "enum value(" << boost::get<int64_t>(constant.value) << ")" << endl;
    else if(constant.type == AstValueType::Boolean)  {
        cout << "boolean(";
        if(boost::get<bool>(constant.value) == true)
            cout << "true";
        else
            cout << "false";
        cout << ")" << endl;
    }
    else
        cerr << "Unknown AST constant value type in print visitor!" << endl;
}

void AstPrintVisitor::visit(AstId &id) {
    //indent the output if needed
    if(indent)
        cout << setw(indent) << ' ';

    if(id.isEnum)
        cout << "enum ";

    cout << "id(";

    if(id.type == AstValueType::Integer)
        cout << "int";
    else if(id.type == AstValueType::Float)
        cout << "float";
    else if(id.type == AstValueType::String)
        cout << "string";
    else if(id.type == AstValueType::Boolean)
        cout << "boolean";
    else if(id.type == AstValueType::Unknown)
        cout << "unknown";
    else
        cerr << "Unknown AST constant value type in print visitor!" << endl;

    cout << ", " << id.position << ": \"" << id.name << "\")" << endl;
}

void AstPrintVisitor::visit(AstFunction &function) {
    //indent the output if needed
    if(indent)
        cout << setw(indent) << ' ';

    cout << "function(";

    if(function.type == AstValueType::Integer)
        cout << "int";
    else if(function.type == AstValueType::Float)
        cout << "float";
    else if(function.type == AstValueType::String)
        cout << "string";
    else if(function.type == AstValueType::Boolean)
        cout << "boolean";
    else if(function.type == AstValueType::Unknown)
        cout << "unknown";
    else
        cerr << "Unknown AST functino return type in print visitor!" << endl;

    cout << ", " << function.position << ": \"" << function.name << "\")" << endl;

    //also print the params
    for(vector<AstConstant *>::iterator it = function.parameters.begin(); it != function.parameters.end(); ++it)  {
        //indent the output if needed
        cout << setw(indent + 4) << ' ' << "parameter(";

        if((*it)->type == AstValueType::Unknown)
            cout << "unknown, " << boost::get<string>((*it)->value);
        else if((*it)->type == AstValueType::Integer)
            cout << "int, " << boost::get<int64_t>((*it)->value);
        else if((*it)->type == AstValueType::Float)
            cout << "float, " << boost::get<double>((*it)->value);
        else if((*it)->type == AstValueType::String)
            cout << "string, \"" << boost::get<string>((*it)->value) << "\"";
        else if((*it)->type == AstValueType::EnumValue)
            cout << "enum value, " << boost::get<int64_t>((*it)->value);
        else if((*it)->type == AstValueType::Boolean)  {
            cout << "boolean, ";
            if(boost::get<bool>((*it)->value) == true)
                cout << "true";
            else
                cout << "false";
        }
        else
            cerr << "Unknown function parameter type in print visitor!" << endl;

        cout << ")" << endl;
    }
}

void AstPrintVisitor::visit(Ast &ast) {
    cout << "Printing the AST:" << endl;
    indent = 0;
    ast.root->accept(*this);
}
