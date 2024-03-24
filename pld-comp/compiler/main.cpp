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
#include "BackendASM.hpp"

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

    // on génère l'IR
    Visitor v;
    v.visit(tree);     
    
    // on récupère l'IR
    IR graphs = v.get_graphs(); 

    // on génère le backend
    std::ofstream file(output);
    BackendASM backend(graphs, output.empty() ? std::cout : file);
    backend.generate();

    return 0;
}
