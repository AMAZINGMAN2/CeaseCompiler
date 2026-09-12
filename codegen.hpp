#pragma once
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <variant>
#include "lexer.hpp"
#include "parser.hpp"
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
    std::cerr<<"Undefined Variable: " << node.name.value.value()<< std::endl;
    exit(1);
  }
  return i->second; // returns the value of the variable
}


int eval(const binExpr& node);
int eval(const binExpr* node);

// takes care of the Expr variant by splitting its evals to the correct function
int eval(const Expr& node)
{
  return std::visit([](const auto& expr){return eval(expr);}, node);
}



int eval(const binExpr& node)
{
  // return node.value; // returns the integer
  if(node.op.type == TokenType::star) 
  {
    return eval(node.left) * eval(node.right);
  }
  if(node.op.type == TokenType::plus) 
  {

    return eval(node.left) + eval(node.right);
  }
  if(node.op.type == TokenType::minus) 
  {

    return eval(node.left) - eval(node.right);
  }
  if(node.op.type == TokenType::fslash) 
  {

    return eval(node.left) / eval(node.right);
  }
  expected("Binary Expression");
}

// dereferences the binExpr if its a pointer to call the function above
int eval(const binExpr* node)
{
  return eval(*node);
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


