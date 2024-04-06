#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

#include "CFG.hpp"
#include <map>

class Visitor : public ifccBaseVisitor
{
public:
    Visitor();
    virtual ~Visitor();

    IR get_graphs();

    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitLvalue(ifccParser::LvalueContext* ctx) override;
    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext* ctx) override;
    virtual antlrcpp::Any visitAssignation(ifccParser::AssignationContext* ctx) override;
    virtual antlrcpp::Any visitExprConstante(ifccParser::ExprConstanteContext* ctx) override;
    virtual antlrcpp::Any visitExprAddSub(ifccParser::ExprAddSubContext* ctx) override;
    virtual antlrcpp::Any visitExprLvalue(ifccParser::ExprLvalueContext* ctx) override;
    virtual antlrcpp::Any visitExprMultDiv(ifccParser::ExprMultDivContext* ctx) override;
    virtual antlrcpp::Any visitExprUnaire(ifccParser::ExprUnaireContext* ctx) override;
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration(ifccParser::DeclarationContext* ctx) override;
    virtual antlrcpp::Any visitLoop(ifccParser::LoopContext* ctx) override;
    virtual antlrcpp::Any visitCondition(ifccParser::ConditionContext* ctx) override;
    virtual antlrcpp::Any visitExprAnd(ifccParser::ExprAndContext* ctx) override;
    virtual antlrcpp::Any visitExprOr(ifccParser::ExprOrContext* ctx) override;
    virtual antlrcpp::Any visitExprNot(ifccParser::ExprNotContext* ctx) override;
    virtual antlrcpp::Any visitExprCmp(ifccParser::ExprCmpContext* ctx) override;

private:
    void          open_scope();
    void          close_scope();
    const Symbol& pop_symbol();
    void          reduce_not();
    void          reduce_and();
    void          reduce_or();

    Scope*              current_scope; ///< current scope
    std::vector<Scope*> scopes;        ///< scopes
    std::stack<Symbol>  symbols;       ///< symboles pour la réduction
    IR                  graphs;        ///< graphs
};
