#include "ast_policy_compressor_visitor.hh"

void AstPolicyCompressorVisitor::visit(AstOperation &op) {
    //check that the operation is not the unsupported negation (is moved into the relations)
    if(op.type == AstOperationType::NOT)
        throw "The not operation should be eliminated at the policy compression stage, since we here only support & and |.";

    //store the AND and OR operations directly inside the RPN stack
    if(op.type == AstOperationType::OR || op.type == AstOperationType::AND)  {
        //call the policy compressor also for the children of the node
        //from left to right
        for(uint8_t i = 0; i < op.getNumberOfChildren(); i++)  {
            op.getChild(i)->accept(*this);
        }

        //store the polcy in the reverse order as the RPN s.t. we can build the execution stack from first to last bit
        if(op.type == AstOperationType::OR)  {
            StackOperation stackOp;
            stackOp.type = PolicyStackOperationType::OR;
            policyStack.push_back(stackOp);
        } else { //must be AND
            StackOperation stackOp;
            stackOp.type = PolicyStackOperationType::AND;
            policyStack.push_back(stackOp);
        }
    }
    else if(op.type == AstOperationType::ELIMINATED_NOT)  {
        assert(op.getNumberOfChildren() == 1); //only NOT with one child are supported
        op.getChild(0)->accept(*this);
        #if DEBUG_POLICY_GENERATION
            cout << " Eliminated NOT ";
        #endif
    }
    //the remaining operations are all relations
    else  {
        //add the children to the relation
        op.getChild(0)->accept(*this);
        if(op.type != AstOperationType::IS_TRUE && op.type != AstOperationType::IS_FALSE)  {
            assert(op.getNumberOfChildren() == 2); //we support only non binary operations, for IS_TRUE and IS_FALSE
            op.getChild(1)->accept(*this);
        }

        //convert to AstOperationType to RelationSetType
        RelationSetType relationType;
        if(op.type == AstOperationType::EQUAL)
            relationType = RelationSetType::EQUAL;
        else if(op.type == AstOperationType::NEQ)
            relationType = RelationSetType::NEQ;
        else if(op.type == AstOperationType::LESS)
            relationType = RelationSetType::LESS;
        else if(op.type == AstOperationType::LEQ)
            relationType = RelationSetType::LEQ;
        else if(op.type == AstOperationType::GREATER)
            relationType = RelationSetType::GREATER;
        else if(op.type == AstOperationType::GEQ)
            relationType = RelationSetType::GEQ;
        else if(op.type == AstOperationType::IS_TRUE)
            relationType = RelationSetType::IS_TRUE;
        else if(op.type == AstOperationType::IS_FALSE)
            relationType = RelationSetType::IS_FALSE;
        else
            throw "Compression for this RelationSetType is not implemented";
        int64_t relationId = relationSet.addType(relationType);
        #if DEBUG_POLICY_GENERATION
            cout << " RelId:" << relationId;
        #endif

        //insert the new relation
        StackOperation stackOp;
        if(relationId == -1) //a normal, new relation
            stackOp.type = PolicyStackOperationType::NEXT_RELATION;
        else  { //a redundant relation s.t. we use the specific relation feature to point to the previously stored one
            stackOp.type = PolicyStackOperationType::SPECIFIC_RELATION;
            stackOp.relationId = relationId;
        }
        policyStack.push_back(stackOp); //add the relation to the stack
    }
}

void AstPolicyCompressorVisitor::visit(AstConstant &constant) {
    //insert new relation inside the equation set
    relationSet.addRelationElement(constant);
}

void AstPolicyCompressorVisitor::visit(AstId &id) {
    //insert new relation inside the equation set
    relationSet.addRelationElement(id);
}

void AstPolicyCompressorVisitor::visit(AstFunction &func) {
    //insert new relation inside the equation set
    relationSet.addRelationElement(func);
}

void AstPolicyCompressorVisitor::visit(Ast &ast) {
    policyStack.addEndDelimiter(); //add the end delimiter to the policy stack end
    ast.root->accept(*this); //check from the root the types
}
