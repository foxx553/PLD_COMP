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

    IR get_ir();

    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;

    virtual antlrcpp::Any visitFunction_global(ifccParser::Function_globalContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration_global(ifccParser::Declaration_globalContext* ctx) override;

    virtual antlrcpp::Any visitBlock_stmt(ifccParser::Block_stmtContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitLoop_stmt(ifccParser::Loop_stmtContext* ctx) override;
    virtual antlrcpp::Any visitCondition_stmt(ifccParser::Condition_stmtContext* ctx) override;
    virtual antlrcpp::Any visitBreak_stmt(ifccParser::Break_stmtContext* ctx) override;
    virtual antlrcpp::Any visitContinue_stmt(ifccParser::Continue_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration_stmt(ifccParser::Declaration_stmtContext* ctx) override;

    virtual antlrcpp::Any visitLvalue(ifccParser::LvalueContext* ctx) override;
    virtual antlrcpp::Any visitAssignment(ifccParser::AssignmentContext* ctx) override;
    virtual antlrcpp::Any visitFunction_call(ifccParser::Function_callContext* ctx) override;

    virtual antlrcpp::Any visitExprConstante(ifccParser::ExprConstanteContext* ctx) override;
    virtual antlrcpp::Any visitExprAddSub(ifccParser::ExprAddSubContext* ctx) override;
    virtual antlrcpp::Any visitExprLvalue(ifccParser::ExprLvalueContext* ctx) override;
    virtual antlrcpp::Any visitExprProduit(ifccParser::ExprProduitContext* ctx) override;
    virtual antlrcpp::Any visitExprUnaire(ifccParser::ExprUnaireContext* ctx) override;
    virtual antlrcpp::Any visitExprFunction(ifccParser::ExprFunctionContext* ctx) override;
    virtual antlrcpp::Any visitExprAssignment(ifccParser::ExprAssignmentContext* ctx) override;
    virtual antlrcpp::Any visitExprNot(ifccParser::ExprNotContext* ctx) override;
    virtual antlrcpp::Any visitExprAnd(ifccParser::ExprAndContext* ctx) override;
    virtual antlrcpp::Any visitExprOr(ifccParser::ExprOrContext* ctx) override;
    virtual antlrcpp::Any visitExprCmp(ifccParser::ExprCmpContext* ctx) override;

private:
    void          open_scope();
    void          close_scope();
    const Symbol& pop_symbol();
    void          reduce_not();
    void          reduce_and();
    void          reduce_or();
    Symbol        reduce_constant(ifccParser::ConstantContext* ctx);
    CFG*          find_graph(const std::string& name) const;

    std::stack<std::pair<BasicBlock*, BasicBlock*>> loops;         ///< loop tree
    Scope*                                          current_scope; ///< current scope
    std::vector<Scope*>                             scopes;        ///< scopes
    std::stack<Symbol>                              symbols;       ///< symboles pour la réduction
    std::vector<CFG*>                               graphs;        ///< graphs
};
