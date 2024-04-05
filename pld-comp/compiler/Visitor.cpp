#include "Visitor.hpp"

#include <iostream>

IR Visitor::get_graphs()
{
    return graphs;
}

Visitor::~Visitor()
{
    for(auto graph: graphs)
    {
        delete graph;
    }
}

antlrcpp::Any Visitor::visitFunction(ifccParser::FunctionContext* ctx)
{
    auto name = ctx->IDENTIFIER()[0]->getText();

    auto current_CFG = new CFG(name);

    graphs.push_back(current_CFG);
    this->visitChildren(ctx);

    current_CFG->current_bb->exit_true = current_CFG->end_bb;

    return 0;
}

antlrcpp::Any Visitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx)
{
    this->visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto dest_idx = graph->get_var_index(pop_expression(graph));

    block->add_IRInstr(Operation::ret, Type::INT_64, {std::to_string(dest_idx)});

    // exit
    block->exit_true = graph->end_bb;

    return 0;
}

antlrcpp::Any Visitor::visitDeclaration(ifccParser::DeclarationContext* ctx)
{
    antlr4::tree::TerminalNode* identifier = ctx->assignation() ? ctx->assignation()->IDENTIFIER() : ctx->IDENTIFIER();

    graphs.back()->add_to_symbol_table(identifier->getText(), Type::INT_64);

    if(ctx->assignation())
    {
        this->visit(ctx->assignation());
    }

    return 0;
}

antlrcpp::Any Visitor::visitAssignation(ifccParser::AssignationContext* ctx)
{
    this->visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto dest = graph->get_var_index(ctx->IDENTIFIER()->getText());
    auto source = graph->get_var_index(pop_expression(graph));

    std::vector<std::string> params{std::to_string(dest), std::to_string(source)};
    block->add_IRInstr(Operation::copy, Type::INT_64, params);

    return 0;
}

antlrcpp::Any Visitor::visitExprConstante(ifccParser::ExprConstanteContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto [dest_name, dest_idx] = graph->create_new_tempvar(Type::INT_64);
    int value = std::stoi(ctx->CONST()->getText());

    block->add_IRInstr(Operation::ldconst, Type::INT_64, {std::to_string(dest_idx), std::to_string(value)});

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any Visitor::visitExprAddSub(ifccParser::ExprAddSubContext* ctx)
{
    visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto droite = graph->get_var_index(pop_expression(graph));
    auto gauche = graph->get_var_index(pop_expression(graph));

    auto [dest_name, dest_idx] = graph->create_new_tempvar(Type::INT_64);

    std::vector<std::string> params{std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)};
    block->add_IRInstr(ctx->ADD() ? Operation::add : Operation::sub, Type::INT_64, params);

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any Visitor::visitExprMultDiv(ifccParser::ExprMultDivContext* ctx)
{
    visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto droite = graph->get_var_index(pop_expression(graph));
    auto gauche = graph->get_var_index(pop_expression(graph));

    auto [dest_name, dest_idx] = graph->create_new_tempvar(Type::INT_64);

    std::vector<std::string> params{std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)};
    block->add_IRInstr(ctx->MUL() ? Operation::mul : Operation::div, Type::INT_64, params);

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any Visitor::visitExprVariable(ifccParser::ExprVariableContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto [dest_name, dest_idx] = graph->create_new_tempvar(Type::INT_64);
    auto source = graph->get_var_index(ctx->IDENTIFIER()->getText());

    block->add_IRInstr(Operation::copy, Type::INT_64, {std::to_string(dest_idx), std::to_string(source)});

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any Visitor::visitExprUnaire(ifccParser::ExprUnaireContext* ctx)
{
    visitChildren(ctx);

    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    auto [facteur_name, facteur_idx] = graph->create_new_tempvar(Type::INT_64);
    block->add_IRInstr(Operation::ldconst, Type::INT_64, {std::to_string(facteur_idx), ctx->ADD() ? "1" : "-1"});

    auto source = graph->get_var_index(pop_expression(graph));

    auto [dest_name, dest_idx] = graph->create_new_tempvar(Type::INT_64);
    block->add_IRInstr(Operation::mul, Type::INT_64, {std::to_string(dest_idx), std::to_string(source), std::to_string(facteur_idx)});

    expressions.push(dest_name);

    return 0;
}

antlrcpp::Any Visitor::visitCall_stmt(ifccParser::Call_stmtContext* ctx)
{
    auto ctxf = ctx->function_call();
    visitFunction_call(ctxf);
    return 0;
}

antlrcpp::Any Visitor::visitFunction_call(ifccParser::Function_callContext* ctx)
{
    auto* graph = graphs.back();
    auto* block = graph->current_bb;
    auto name = ctx->IDENTIFIER()->getText();
    std::vector<std::string> parameters;
    parameters.push_back(name);

    for(int i = 0; i < ctx->expression().size(); i++)
    {
        this->visit(ctx->expression()[i]);
        parameters.push_back(pop_expression(graph));
    }
    
    block->add_IRInstr(Operation::call, Type::INT_64, parameters);
        
    return 0;
}
antlrcpp::Any Visitor::visitLoop(ifccParser::LoopContext* ctx)
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating bb after the 'while'
    auto out_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(out_bb);

    // 'while' condition
    auto expression = ctx->expression();
    auto condition_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(condition_bb);

    // Branching the previous block to the 'while' condition
    block->exit_true = condition_bb;

    // Visiting the condition expression
    graph->current_bb = condition_bb;
    this->visit(expression);
    condition_bb = graph->current_bb; // on récupère le "bloc out" de l'expression

    condition_bb->test_var_name = pop_expression(graph);

    // 'while' block
    auto block_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(block_bb);

    // Branching the condition to the beginning of the block
    condition_bb->exit_true = block_bb;

    // Branching the 'while' block to the condition bb
    graph->current_bb = block_bb;
    this->visit(ctx->block());
    block_bb = graph->current_bb;  // on récupère le "bloc out" du bloc
    block_bb->exit_true = condition_bb;

    // bb linking
    condition_bb->exit_false = out_bb;

    // Setting current bb to the out bb
    graph->current_bb = out_bb;

    return 0;
}

antlrcpp::Any Visitor::visitCondition(ifccParser::ConditionContext* ctx)
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating bb after all the conditions
    auto out_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(out_bb);

    BasicBlock* last_condition_bb = nullptr;

    // Following blocks
    for(int i = 0; i < ctx->expression().size(); i++)
    {
        // 'if' condition
        auto expression = ctx->expression()[i];

        auto condition_bb = new BasicBlock(graph, graph->new_BB_name());
        graph->add_bb(condition_bb);

        // Branching the first bb
        if(i == 0)
        {
            block->exit_true = condition_bb;
        }

        graph->current_bb = condition_bb;
        this->visit(expression);
        condition_bb = graph->current_bb; // on récupère le "bloc out" de l'expression

        condition_bb->test_var_name = pop_expression(graph);

        if(last_condition_bb)
        {
            last_condition_bb->exit_false = condition_bb;
        }

        // 'if' block
        auto block_bb = new BasicBlock(graph, graph->new_BB_name());
        graph->add_bb(block_bb);
        block_bb->exit_true = out_bb;

        graph->current_bb = block_bb;
        this->visit(ctx->block()[i]);
        block_bb = graph->current_bb;  // on récupère le "bloc out" du bloc

        // bb linking
        condition_bb->exit_true = block_bb;
        condition_bb->exit_false = out_bb;

        // last condition bb
        last_condition_bb = condition_bb;
    }

    // There's an 'else' block
    if(ctx->block().size() > ctx->expression().size())
    {
        auto else_block = ctx->block()[ctx->expression().size()];

        auto else_bb = new BasicBlock(graph, graph->new_BB_name());
        graph->add_bb(else_bb);

        graph->current_bb = else_bb;
        this->visit(else_block);

        last_condition_bb->exit_false = else_bb;
        else_bb->exit_true = out_bb;
    }

    // Setting current bb to the out bb
    graph->current_bb = out_bb;

    return 0;
}

antlrcpp::Any Visitor::visitExprAnd(ifccParser::ExprAndContext* ctx)
{
    // Get results of left and right
    visitChildren(ctx);

    reduce_and();

    return 0;
}

antlrcpp::Any Visitor::visitExprOr(ifccParser::ExprOrContext* ctx)
{
    // Get results of left and right
    visitChildren(ctx);

    reduce_or();

    return 0;
}

antlrcpp::Any Visitor::visitExprNot(ifccParser::ExprNotContext* ctx)
{
    // Get value
    visitChildren(ctx);

    reduce_not();

    return 0;
}

antlrcpp::Any Visitor::visitExprCmp(ifccParser::ExprCmpContext* ctx)
{
    visitChildren(ctx);

    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Destination variable
    auto [dest_name, dest_idx] = graph->create_new_tempvar(Type::INT_64);

    // Get results of left and right
    auto droite = graph->get_var_index(pop_expression(graph));
    auto gauche = graph->get_var_index(pop_expression(graph));

    if(ctx->EQ())
    {
        block->add_IRInstr(Operation::cmp_eq, Type::INT_64, {std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)});
        expressions.push(dest_name);
    }
    else if(ctx->LT())
    {
        block->add_IRInstr(Operation::cmp_lt, Type::INT_64, {std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)});
        expressions.push(dest_name);
    }
    else if(ctx->NE())
    {
        block->add_IRInstr(Operation::cmp_eq, Type::INT_64, {std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)});
        expressions.push(dest_name);
        reduce_not();
    }
    else if(ctx->GE())
    {
        block->add_IRInstr(Operation::cmp_lt, Type::INT_64, {std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)});
        expressions.push(dest_name);
        reduce_not();
    }
    else if(ctx->LE())
    {
        block->add_IRInstr(Operation::cmp_eq, Type::INT_64, {std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)});
        expressions.push(dest_name);

        auto [second_dest_name, second_dest_idx] = graph->create_new_tempvar(Type::INT_64);
        block->add_IRInstr(Operation::cmp_lt, Type::INT_64, {std::to_string(second_dest_idx), std::to_string(gauche), std::to_string(droite)});
        expressions.push(second_dest_name);

        reduce_or();
    }
    else if(ctx->GT())
    {
        block->add_IRInstr(Operation::cmp_eq, Type::INT_64, {std::to_string(dest_idx), std::to_string(gauche), std::to_string(droite)});
        expressions.push(dest_name);

        auto [second_dest_name, second_dest_idx] = graph->create_new_tempvar(Type::INT_64);
        block->add_IRInstr(Operation::cmp_lt, Type::INT_64, {std::to_string(second_dest_idx), std::to_string(gauche), std::to_string(droite)});
        expressions.push(second_dest_name);

        reduce_or();
        reduce_not();
    }

    return 0;
}

void Visitor::reduce_and()
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating out block
    auto out_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(out_bb);

    // Get values
    auto droite = pop_expression(graph);
    auto gauche = pop_expression(graph);

    // Destination variable
    auto [dest_name, dest_idx] = graph->create_new_tempvar(Type::INT_64);

    // True / false blocks
    auto true_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(true_bb);
    true_bb->add_IRInstr(Operation::ldconst, Type::INT_64, {std::to_string(dest_idx), "1"});
    true_bb->exit_true = out_bb;

    auto false_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(false_bb);
    false_bb->add_IRInstr(Operation::ldconst, Type::INT_64, {std::to_string(dest_idx), "0"});
    false_bb->exit_true = out_bb;

    // Conditions
    auto droite_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(droite_bb);
    droite_bb->test_var_name = droite;
    droite_bb->exit_true = true_bb;
    droite_bb->exit_false = false_bb;

    auto gauche_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(gauche_bb);
    gauche_bb->test_var_name = gauche;
    gauche_bb->exit_true = droite_bb;
    gauche_bb->exit_false = false_bb;

    expressions.push(dest_name);

    // Init
    block->exit_true = gauche_bb;
    graph->current_bb = out_bb;
}

void Visitor::reduce_or()
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating out block
    auto out_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(out_bb);

    // Get values
    auto droite = pop_expression(graph);
    auto gauche = pop_expression(graph);

    // Destination variable
    auto [dest_name, dest_idx] = graph->create_new_tempvar(Type::INT_64);

    // True / false blocks
    auto true_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(true_bb);
    true_bb->add_IRInstr(Operation::ldconst, Type::INT_64, {std::to_string(dest_idx), "1"});
    true_bb->exit_true = out_bb;

    auto false_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(false_bb);
    false_bb->add_IRInstr(Operation::ldconst, Type::INT_64, {std::to_string(dest_idx), "0"});
    false_bb->exit_true = out_bb;

    // Conditions
    auto droite_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(droite_bb);
    droite_bb->test_var_name = droite;
    droite_bb->exit_true = true_bb;
    droite_bb->exit_false = false_bb;

    auto gauche_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(gauche_bb);
    gauche_bb->test_var_name = gauche;
    gauche_bb->exit_true = true_bb;
    gauche_bb->exit_false = droite_bb;

    expressions.push(dest_name);

    // Init
    block->exit_true = gauche_bb;
    graph->current_bb = out_bb;
}

void Visitor::reduce_not()
{
    // Getting current CFG
    auto* graph = graphs.back();
    auto* block = graph->current_bb;

    // Creating out block
    auto out_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(out_bb);

    // Get value
    auto value = pop_expression(graph);

    // Destination variable
    auto [dest_name, dest_idx] = graph->create_new_tempvar(Type::INT_64);

    // True / false blocks
    auto true_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(true_bb);
    true_bb->add_IRInstr(Operation::ldconst, Type::INT_64, {std::to_string(dest_idx), "1"});
    true_bb->exit_true = out_bb;

    auto false_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(false_bb);
    false_bb->add_IRInstr(Operation::ldconst, Type::INT_64, {std::to_string(dest_idx), "0"});
    false_bb->exit_true = out_bb;

    // Condition
    auto not_bb = new BasicBlock(graph, graph->new_BB_name());
    graph->add_bb(not_bb);
    not_bb->test_var_name = value;
    not_bb->exit_true = false_bb;
    not_bb->exit_false = true_bb;

    expressions.push(dest_name);

    // Init
    block->exit_true = not_bb;
    graph->current_bb = out_bb;
}

std::string Visitor::pop_expression(CFG* graph)
{
    std::string value = expressions.top();
    expressions.pop();
    return value;
}