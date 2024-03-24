#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "IRInstr.hpp"
#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "generated/ifccLexer.h"
#include "generated/ifccParser.h"

#include "Visitor.hpp"
#include "Backend.hpp"

using namespace antlr4;
using namespace std;

int main(int argn, const char** argv)
{
    std::string output;

    stringstream in;
    if(argn >= 2)
    {
        ifstream lecture(argv[1]);
        in << lecture.rdbuf();

        if(argn == 3)
        {
            output = argv[2]; 
        }
    }
    else
    {
        cerr << "usage: ifcc path/to/file.c" << endl;
        exit(1);
    }

    ANTLRInputStream input(in.str());

    ifccLexer         lexer(&input);
    CommonTokenStream tokens(&lexer);

    tokens.fill();

    ifccParser       parser(&tokens);
    tree::ParseTree* tree = parser.axiom();

    if(parser.getNumberOfSyntaxErrors() != 0)
    {
        cerr << "error: syntax error during parsing" << endl;
        exit(1);
    }

    // TEST IRINSTR
    // std::vector<std::string> params{"4","5","3"};
    // IRInstr instr= IRInstr(Operation::mul, Type::INT_64,params);
    // std::ostream &o = std::cout;
    // instr.gen_asm(o);

    // Output to file
    /* 
    std::string name(argv[1]);
    name[name.size() - 1] = 's';
    std::ofstream output(name); 
    */

    Visitor v;
    v.visit(tree);     // on génère l'IR
    
    IR graphs = v.get_graphs(); // on récupère l'IR

    BackendASM backend(graphs);

    std::ofstream file(output);
    backend.generate(output.empty() ? std::cout : file);

    return 0;
}
