#pragma once
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <variant>
#include "lexer.hpp"
#include "parser.hpp"
auto program = parse();
std::unordered_map<std::string, int> variables;

int eval(const intLit& node)
{
  // return node.value; // returns the integer
  return std::stoi(node.value.value.value()); // returns the integer

}
int eval(const identLit& node)
{
  auto i = variables.find(node.name.value.value());
  if (i == variables.end()) {
    std::cerr<<"Undefined Variable" << node.name.value.value()<< std::endl;
  }
  return i->second; // returns the value of the variable
}
void generator(const exitstmt& node)
{
  std::visit([](auto& value)
      {
        exit(eval(value));
      }, node.value);
}
void generator(const letstmt& node)
{
  variables[node.name.value.value()] = std::visit([](auto& value)
      {
        return eval(value);
      }, node.value);
}

void generator(const Program&  program)
{
  for(const auto& statement : program.statements)
  {
    std::visit([](auto& stmt){
        generator(stmt);
        },statement);
  }
}

//
//
// if(program.value.value())
// {
//   ;;
// }



void _exit(int n)
{
  exit(n);
}


