/*
* AST class that represets an abstract syntax tree
*
* Author: Sascha Schmerling
* Created: 2015/06/02
*/

#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <boost/variant.hpp>
#include <boost/algorithm/string.hpp>

#include "options.hh"

using namespace std;

class Ast;
class AstVisitor;
class AstVisitableNode;
class AstOperation;
class AstConstant;
class AstId;
class AstFunction;

//Visitor pattern
//base class for the visitor that will used to get the type of a node
class AstVisitor {
    public: 
        virtual void visit(AstOperation &) = 0;
        virtual void visit(AstConstant &) = 0;
        virtual void visit(AstId &) = 0;
        virtual void visit(AstFunction &) = 0;
        virtual void visit(Ast &) = 0;

        virtual ~AstVisitor() {};
};

enum class AstNodeType : uint8_t {
    Operation,
    Constant,
    Id,
    Function
};

//Visitor pattern
//base class that allows use to traverse through the AST
//with a generic base type
class AstVisitableNode  {
    public:
        virtual void accept(AstVisitor &v) = 0;
        virtual ~AstVisitableNode() {}
        AstNodeType nodeType;
};

class Ast  {
    public:
        AstVisitableNode *root;

        Ast(AstVisitableNode *initRoot)  {
            root = initRoot;
        }
};

//all types with that will have a binary node (two children)
enum class AstOperationType : uint8_t {
    ELIMINATED_NOT, //status that eliminated NOTs will become
    NOT,
    AND,
    OR,
    EQUAL,
    NEQ,
    LESS,
    LEQ,
    GREATER,
    GEQ,
    IS_TRUE,
    IS_FALSE,
};

//a node that represents an operation with a list of children
class AstOperation : public AstVisitableNode {
    public: 
        AstOperationType type;

        //constructor for a operation with only one child
        AstOperation(AstOperationType initType, AstVisitableNode *left)  {
            nodeType = AstNodeType::Operation;
            type = initType;

            addChild(left);
        }

        //constructor for a binary operation (two children)
        AstOperation(AstOperationType initType, AstVisitableNode *left, AstVisitableNode *right)  {
            nodeType = AstNodeType::Operation;
            type = initType;

            addChild(left);
            addChild(right);
        }

        uint8_t getNumberOfChildren()  {
            return children.size();
        }

        vector<AstVisitableNode *> &getChildren() {
            return children;
        }

        AstVisitableNode *getChild(uint8_t pos)  {
            return children.at(pos);
        }

        void setChild(uint8_t pos, AstVisitableNode *node)  {
            children.at(pos) = node;
        }

        void addChild(AstVisitableNode *child)  {
            return children.push_back(child);
        }

        void accept(AstVisitor &visitor)  {
            visitor.visit(*this);
        }

        //delete all children when the node gets destroyed
        ~AstOperation()  {
            for(vector<AstVisitableNode *>::iterator it = children.begin(); it != children.end(); ++it)
                delete *it;
        }

        vector<AstVisitableNode *> children;
};

enum class AstValueType : uint8_t {
	INT8 = 0,
	INT16 = 1,
	INT32 = 2,
	INT64 = 3,
	UINT8 = 4,
	UINT16 = 5,
	UINT32 = 6,
	UINT64 = 7,
	Float = 8,
	Boolean = 9,
	String = 10,
	EnumValue = 11,
    Unknown = 12,
    Integer = 13,
    Id = 14, //just used for the IS_TRUE, IS_FALSE compression feature
};

typedef boost::variant<bool, int64_t, double, string> AstValue;

//Ast leaf for a number
class AstConstant : public AstVisitableNode  {
    public: 
        //create a tagged union for the type
        //we need to check every time the type
        //before we access the union, since
        //else we could recieve wrong values
        AstValueType type;
        AstValue value;
        id_type enumValuePosition;

        AstConstant(AstValueType type, AstValue value) : type(type), value(value) {
            nodeType = AstNodeType::Constant;
        }

        void accept(AstVisitor &visitor)  {
            visitor.visit(*this);
        }
};

//Ast leaf for an id
class AstId : public AstVisitableNode  {
    public: 
        AstValueType type;
        string name;
        id_type position;
        bool isEnum;

        AstId(AstValueType type, string name) : type(type), name(name), position(0)  {
            nodeType = AstNodeType::Id;
        }

        void accept(AstVisitor &visitor)  {
            visitor.visit(*this);
        }
};

//Ast leaf for a function
class AstFunction : public AstVisitableNode  {
    public: 
        AstValueType type;
        string name;
        vector<AstConstant *> parameters;
        id_type position;

        AstFunction(string callString) : position(0)  {
            nodeType = AstNodeType::Function;
            type = AstValueType::Boolean; //currently just functions with a boolean return type are supported

            //extract the function id from the callString
            name = callString.substr(0, callString.find('('));

            string rawParameters = callString.substr(callString.find('(') + 1); //remove the left bracket
            rawParameters = rawParameters.substr(0, rawParameters.find(')')); //remove the right bracket
            for(size_t curPos = 0; curPos < rawParameters.size(); )  {
                //get the current raw parameter
                string curRawParameter;
                size_t commaPos = rawParameters.find(',', curPos);
                if(commaPos != string::npos)  { //get next param (comma is seperator)
                    curRawParameter = rawParameters.substr(curPos, commaPos - curPos);
                    curPos = commaPos + 1;
                }
                else   {//last parameter, since no following comma
                    curRawParameter = rawParameters.substr(curPos, rawParameters.size());
                    curPos += curRawParameter.size();
                }
                //remove unwanted whitespaces
                boost::algorithm::trim(curRawParameter);

                //just store the param with type unknown as string
                //the preprocessor will check and convert the value
                AstConstant *param = new AstConstant(AstValueType::Unknown, curRawParameter);
                parameters.push_back(param);
            }
        }

        void accept(AstVisitor &visitor)  {
            visitor.visit(*this);
        }
};
