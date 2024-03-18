#include "CodeGenVisitor.h"

void CodeGenVisitor::gen_asm(std::ostream& o)
{
    o << ".text" << std::endl;
    o << ".globl main" << std::endl;

    for(auto graph: graphs)
    {
        graph->gen_asm(o);
    }
}

CodeGenVisitor::~CodeGenVisitor()
{
    for(auto graph: graphs)
    {
        delete graph;
    }
}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext* ctx)
{
    this->visitChildren(ctx);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitFunction(ifccParser::FunctionContext* ctx)
{
    auto name = ctx->IDENTIFIER()[0]->getText();

    graphs.push_back(new CFG(name));
    this->visit(ctx->instruction());

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    this->visitChildren(ctx);

    auto dest_idx = graph->get_var_index(expressions.top());
    expressions.pop();

    block->add_IRInstr(IRInstr::Operation::ret, Type::INT_64, {std::to_string(dest_idx)});

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclare_stmt(ifccParser::Declare_stmtContext* ctx)
{
    this->visitChildren(ctx);
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclaration(ifccParser::DeclarationContext* ctx)
{
    antlr4::tree::TerminalNode* identifier = ctx->assignation() ? ctx->assignation()->IDENTIFIER() : ctx->IDENTIFIER();

    graphs.back()->add_to_symbol_table(identifier->getText(), Type::INT_64);

    if(ctx->assignation())
    {
        this->visit(ctx->assignation());
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext* ctx)
{
    this->visitChildren(ctx);
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssignation(ifccParser::AssignationContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    this->visitChildren(ctx);

    auto dest = graph->get_var_index(ctx->IDENTIFIER()->getText());
    auto source = graph->get_var_index(expressions.top());
    expressions.pop();

    std::vector<std::string> params{std::to_string(dest), std::to_string(source)};
    block->add_IRInstr(IRInstr::Operation::copy, Type::INT_64, params);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprConstante(ifccParser::ExprConstanteContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto dest_name = graph->create_new_tempvar(Type::INT_64);
    auto dest_idx = graph->get_var_index(dest_name);
    int  value = std::stoi(ctx->CONST()->getText());

    block->add_IRInstr(IRInstr::Operation::ldconst, Type::INT_64, {std::to_string(dest_idx), std::to_string(value)});

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprAddSub(ifccParser::ExprAddSubContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    visitChildren(ctx);

    auto droite = graph->get_var_index(expressions.top());
    expressions.pop();
    auto gauche = graph->get_var_index(expressions.top());
    expressions.pop();

    auto dest_name = graph->create_new_tempvar(Type::INT_64);
    auto dest_idx = graph->get_var_index(dest_name);

    std::vector<std::string> params{std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)};
    block->add_IRInstr(ctx->ADD() ? IRInstr::Operation::add : IRInstr::Operation::sub, Type::INT_64, params);

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprMultDiv(ifccParser::ExprMultDivContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    visitChildren(ctx);

    auto droite = graph->get_var_index(expressions.top());
    expressions.pop();
    auto gauche = graph->get_var_index(expressions.top());
    expressions.pop();

    auto dest_name = graph->create_new_tempvar(Type::INT_64);
    auto dest_idx = graph->get_var_index(dest_name);

    std::vector<std::string> params{std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)};
    block->add_IRInstr(ctx->MUL() ? IRInstr::Operation::mul : IRInstr::Operation::div, Type::INT_64, params);

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprVariable(ifccParser::ExprVariableContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto dest_name = graph->create_new_tempvar(Type::INT_64);
    auto dest_idx = graph->get_var_index(dest_name);
    auto source = graph->get_var_index(ctx->IDENTIFIER()->getText());

    block->add_IRInstr(IRInstr::Operation::copy, Type::INT_64, {std::to_string(dest_idx), std::to_string(source)});

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExprUnaire(ifccParser::ExprUnaireContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    visitChildren(ctx);

    auto facteur = graph->get_var_index(graph->create_new_tempvar(Type::INT_64));
    block->add_IRInstr(IRInstr::Operation::ldconst, Type::INT_64, {std::to_string(facteur), std::to_string(ctx->ADD() ? 1 : -1)});

    auto source = graph->get_var_index(expressions.top());
    expressions.pop();

    auto dest_name = graph->create_new_tempvar(Type::INT_64);
    auto dest_idx = graph->get_var_index(dest_name);
    block->add_IRInstr(IRInstr::Operation::mul, Type::INT_64, {std::to_string(dest_idx), std::to_string(source), std::to_string(facteur)});

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitCall_stmt(ifccParser::Call_stmtContext* ctx)
{
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitFunction_call(ifccParser::Function_callContext* ctx)
{
    return 0;
}