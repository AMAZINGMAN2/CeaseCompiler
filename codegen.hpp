#pragma once
#include <cstdlib>
#include <unordered_map>
#include "lexer.hpp"
#include "parser.hpp"
auto program = parse();
std::unordered_map<std::string, int> variables;

// int eval(const intLit& node)
// {
//   return node.value.value.value();
// }
//
//
//
// if(program.value.value())
// {
//   ;;
// }


void generator()
{

}

void _exit(int n)
{
  exit(n);
}


