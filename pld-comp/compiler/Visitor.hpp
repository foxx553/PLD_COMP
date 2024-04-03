#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

#include "CFG.hpp"
#include <map>

class Visitor : public ifccBaseVisitor
{
public:
    virtual ~Visitor();

    IR get_graphs();

    virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitAssignation(ifccParser::AssignationContext* ctx) override;
    virtual antlrcpp::Any visitExprConstante(ifccParser::ExprConstanteContext* ctx) override;
    virtual antlrcpp::Any visitExprAddSub(ifccParser::ExprAddSubContext* ctx) override;
    virtual antlrcpp::Any visitExprVariable(ifccParser::ExprVariableContext* ctx) override;
    virtual antlrcpp::Any visitExprMultDiv(ifccParser::ExprMultDivContext* ctx) override;
    virtual antlrcpp::Any visitExprUnaire(ifccParser::ExprUnaireContext* ctx) override;
    virtual antlrcpp::Any visitCall_stmt(ifccParser::Call_stmtContext* ctx) override;
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext* ctx) override;
    virtual antlrcpp::Any visitLoop(ifccParser::LoopContext* ctx) override;
    virtual antlrcpp::Any visitCondition(ifccParser::ConditionContext* ctx) override;
    virtual antlrcpp::Any visitExprAnd(ifccParser::ExprAndContext* ctx) override;
    virtual antlrcpp::Any visitExprOr(ifccParser::ExprOrContext* ctx) override;
    virtual antlrcpp::Any visitExprNot(ifccParser::ExprNotContext* ctx) override;
    virtual antlrcpp::Any visitExprCmp(ifccParser::ExprCmpContext* ctx) override;

private:
    std::string pop_expression(CFG* graph);
    void reduce_not();
    void reduce_and();
    void reduce_or();

    int                     stack = 0;
    std::stack<std::string> expressions;
    IR                      graphs;
};
