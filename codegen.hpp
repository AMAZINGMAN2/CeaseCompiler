#pragma once
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <variant>
#include "lexer.hpp"
#include "parser.hpp"
struct Cell;
using Value= std::variant<int, bool, double, char, std::shared_ptr<Cell>>; // default float is 64bit and therefore a double
struct Cell
{Value val;};
std::vector<std::unordered_map<std::string, std::shared_ptr<Cell>>> scope(1); // a vector of hashmaps that contain the variables




Value eval(const floatLit& node)
{
  return std::stod(node.value.value.value()); 
}


Value eval(const charLit& node)
{
  return node.value.value.value()[0];
}


Value eval(const intLit& node)
{
  // return node.value; // returns the integer
  return std::stoi(node.value.value.value()); // returns the integer
}

Value eval(const boolLit& node)
{
  return node.value.value.value() == "true";
}



Value eval(const identLit& node)
{
  for(auto i = scope.rbegin(); i != scope.rend(); i++)
  {
    auto j = i->find(node.name.value.value());
    if (j != i->end()) {
      return j->second->val; // returns the value of the variable
    }
  }
  std::cerr<<filename<<":"<<node.name.line<<":"<<node.name.chr<<" error: Undefined Variable '" << node.name.value.value()<<"'"<<std::endl;
  exit(EXIT_FAILURE);
}


std::shared_ptr<Cell> evalCell(const Expr& expr);
Value eval(const binExpr& node);
Value eval(const binExpr* node);
Value eval(const unExpr& node);
Value eval(const unExpr* node);

// takes care of the Expr variant by splitting its evals to the correct function
Value eval(const Expr& node)
{
  return std::visit([](const auto& expr){return eval(expr);}, node);
}


Value eval(const unExpr& node)
{
  if(node.op.type == TokenType::amp) 
  {
    return evalCell(node.value);
  }
  if(node.op.type == TokenType::star) 
  {
    return std::get<std::shared_ptr<Cell>>(eval(node.value))->val;
  }
  if (node.op.type == TokenType::minus)
  {
    return -std::get<int>(eval(node.value));
  }
  exit(EXIT_FAILURE);
}

int evalInt(const Expr& node)
{
  Value value = eval(node);
  if(!std::holds_alternative<int>(value))
  {
    expected("Integer");
  }
  return std::get<int>(value);
}

bool evalBool(const Expr& node)
{
  Value value = eval(node);
  if(!std::holds_alternative<bool>(value))
  {
    expected("Boolean");
  }
  return std::get<bool>(value);
}


Value eval(const unExpr* node)
{
      return eval(*node);
}


Value eval(const binExpr& node)
{
  // return node.value; // returns the integer
  if(node.op.type == TokenType::star) 
  {
    return std::get<int>(eval(node.left)) * std::get<int>(eval(node.right));
  }
  if(node.op.type == TokenType::plus) 
  {

    return std::get<int>(eval(node.left)) + std::get<int>(eval(node.right));
  }
  if(node.op.type == TokenType::minus) 
  {
    return std::get<int>(eval(node.left)) - std::get<int>(eval(node.right));
  }
  if(node.op.type == TokenType::fslash) 
  {
    return std::get<int>(eval(node.left)) / std::get<int>(eval(node.right));
  }
  if(node.op.type == TokenType::greaterthan) 
  {
    return std::get<int>(eval(node.left)) > std::get<int>(eval(node.right));
  }
  if(node.op.type == TokenType::smallerthan) 
  {
    return std::get<int>(eval(node.left)) < std::get<int>(eval(node.right));
  }
  if(node.op.type == TokenType::greaterEq) 
  {
    return std::get<int>(eval(node.left)) >= std::get<int>(eval(node.right));
  }
  if(node.op.type == TokenType::smallerEq) 
  {
    return std::get<int>(eval(node.left)) <= std::get<int>(eval(node.right));
  }
  if(node.op.type == TokenType::eqEq) 
  {
    return eval(node.left) == eval(node.right);
  }
  if(node.op.type == TokenType::notEq) 
  {
    return eval(node.left) != eval(node.right);
  }
  expected("Binary Expression");
  exit(EXIT_FAILURE);
}

// dereferences the binExpr if its a pointer to call the function above
Value eval(const binExpr* node)
{
  return eval(*node);
}

std::shared_ptr<Cell> evalCell(const Expr& expr)
{
  if(std::holds_alternative<identLit>(expr))
  {
    const auto& name = std::get<identLit>(expr).name.value.value();
    for (auto i = scope.rbegin(); i != scope.rend(); i++)
    {
      if(auto j = i->find(name); j != i->end()) 
        return j->second;
    }
  }
  else if(std::holds_alternative<unExpr*>(expr) && std::get<unExpr*>(expr)->op.type == TokenType::star)
  {
    return std::get<std::shared_ptr<Cell>>(eval(std::get<unExpr*>(expr)->value));
  }
  exit(EXIT_FAILURE);
}




void generator(const exitstmt& node)
{
  Value value = eval(node.value);
  if (std::holds_alternative<int>(value))
  {
    exit(std::get<int>(value));
  }
  exit(std::get<bool>(value));
}
void generator(const letstmt& node)
{
  scope.back()[node.name.value.value()] = std::make_shared<Cell>(Cell{eval(node.value)});
}

void generator(const deref& node)
{
  evalCell(node.target)->val = eval(node.value);
}

void generator(const assignstmt& node)
{
  for(auto i = scope.rbegin(); i != scope.rend(); i++)
  {
    auto j = i->find(node.name.value.value());
    if (j != i->end()) {
      j->second->val = eval(node.value); // returns the value of the variable
      return;
    }
  }
  std::cerr<<filename<<":"<<node.name.line<<":"<<node.name.chr<<" error: Undefined Variable '" << node.name.value.value()<<"'"<<std::endl;
  exit(EXIT_FAILURE);
}


void generator(const block* node);

void generator(const ifstmt& node)
{
  for(const auto& branch : node.branches)
  {
    if (evalBool(branch.cond))
    {
      generator(branch.body);
      return;
    }
  }
  if (node.elsebody != nullptr)
  {
    generator(node.elsebody);
  }
}

void generator(const whilestmt& node)
{
  while (evalBool(node.cond))
  {
    generator(node.body);
  }
}


void generator(const forstmt& node)
{
  scope.push_back({}); // extra scope making sure that the loop var doesnt leak
  generator(node.init);
  while (evalBool(node.cond))
  {
    generator(node.body);
    generator(node.update);
  }
  scope.pop_back();
}

void generator(const block* node)
{
  scope.push_back({});
  for(const auto& statement : node->statements)
  {
    std::visit([](auto& stmt){
        generator(stmt);
        },statement);
  }
  scope.pop_back();
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




void _exit(int n)
{
  exit(n);
}


