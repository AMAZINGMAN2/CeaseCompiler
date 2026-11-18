#pragma once
#include <cstdlib>
#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include "parser.hpp"
#include "lexer.hpp"
#include <unordered_map>

enum class dataType
{
  int_, float_, bool_, char_
};

std::string toStr(dataType type);
dataType tokenToType(TokenType token);
void typeError(const token& tok, const std::string& str);
std::vector<std::unordered_map<std::string, dataType>> typeScope(1);

dataType checkExpr(const identLit& node);
dataType checkExpr(const unExpr *node);
dataType checkExpr(const binExpr& node);
dataType checkExpr(const binExpr *node);
dataType checkExpr(const Expr& expr);
void checkStmt(const block* node);
void checkStmt(const forstmt& node);
void checkStmt(const whilestmt& node);
void checkStmt(const assignstmt& node);
void checkStmt(const letstmt& node);
void checkStmt(const ifstmt& node);

dataType checkExpr(const intLit&){return dataType::int_;}
dataType checkExpr(const floatLit&){return dataType::float_;}
dataType checkExpr(const boolLit&){return dataType::bool_;}
dataType checkExpr(const charLit&){return dataType::char_;}
dataType checkExpr(const identLit& node)
{
  for(auto i = typeScope.rbegin(); i != typeScope.rend(); i++)
  {
    auto found = i->find(node.name.value.value());
    if(found != i->end())
    {return found->second;}
  }
  typeError(node.name, "Undefined variable '" + node.name.value.value() + "'");
  return dataType::int_;
}

dataType checkExpr(const unExpr&  node)
{
  if (node.op.type == TokenType::minus)
  {
    dataType inner = checkExpr(node.value);
    if(inner != dataType::int_  && inner != dataType::float_)
    {
    typeError(node.op, "Unary '-' requires int or float, got " + toStr(inner));
    }
    return inner;
  }
  typeError(node.op, "Unsupported unary operator");
  return dataType::int_;
}



dataType checkExpr(const unExpr *node)
{
  return checkExpr(*node);
}




dataType checkExpr(const binExpr& node)
{
  dataType left = checkExpr(node.left);
  dataType right = checkExpr(node.right);
  TokenType op = node.op.type;

if (op == TokenType::plus || op == TokenType::minus || op == TokenType::star || op == TokenType::fslash)
{
  if (left != right || (left != dataType::int_ && left != dataType::float_))
  {
    typeError(node.op, "Invalid arthmetic" + toStr(left) + " and " + toStr(right));
  }
  return left;
}

if (op == TokenType::greaterthan || op == TokenType::smallerthan || op == TokenType::greaterEq || op == TokenType::smallerEq)
{
  if (left != right || left == dataType::bool_)
  {
    typeError(node.op, "Cannot compare" + toStr(left) + " and " + toStr(right));
  }
  return dataType::bool_;
}


if (op == TokenType::eqEq || op == TokenType::notEq)
{
  if (left != right)
  {
    typeError(node.op, "Cannot compare" + toStr(left) + " and " + toStr(right));
  }
  return dataType::bool_;
}

typeError(node.op, "Unknown binary operator");
  return dataType::int_;
}


dataType checkExpr(const binExpr *node)
{
  return checkExpr(*node);
}



dataType checkExpr(const Expr& expr)
{
  return std::visit([](const auto& node){return checkExpr(node);}, expr);
}

void checkStmt(const block* node);

void checkStmt(const letstmt& node)
{
  if(tokenToType(node.type) != checkExpr(node.value))
  {
    typeError(node.name, "Type mismatch of \'"+ node.name.value.value()+"\'");
  }
  if(!typeScope.back().emplace(node.name.value.value(), tokenToType(node.type)).second)
  {
    typeError(node.name, "Redeclaration of \'" + node.name.value.value() + "\'");
  }
}


void checkStmt(const assignstmt& node)
{
  for (auto i = typeScope.rbegin(); i != typeScope.rend(); i++)
  {
    auto found = i->find(node.name.value.value());
    if(found != i->end())
    {
      if(found->second != checkExpr(node.value))
      {
        typeError(node.name,"Cannot assign \'" + toStr(checkExpr(node.value)) + "\'");
      }
      return;
    }
  }
  typeError(node.name, "Undefined variable '" + node.name.value.value() + "'");
}

void checkStmt(const ifstmt& node)
{
  for (const auto& branch : node.branches) {
    if(checkExpr(branch.cond) != dataType::bool_)
    {
      std::cerr << filename << ": error: 'if' condition must be a boolean" << std::endl;
      exit(EXIT_FAILURE);
    }
    checkStmt(branch.body);
  }
  if (node.elsebody) {checkStmt(node.elsebody);}
}


void checkStmt(const whilestmt& node)
{
    if(checkExpr(node.cond)!= dataType::bool_)
    {
      std::cerr << filename << ": error: 'while' condition must be a boolean" << std::endl;
      exit(EXIT_FAILURE);
    }
    checkStmt(node.body);
}

void checkStmt(const forstmt& node)
{
  typeScope.push_back({}); // new scope
  checkStmt(node.init);
  if(checkExpr(node.cond) != dataType::bool_)
  {
      std::cerr << filename << ": error: 'for' condition must be a boolean" << std::endl;
      exit(EXIT_FAILURE);
  }
  checkStmt(node.update);
  checkStmt(node.body);
  typeScope.pop_back();

}

void checkStmt(const exitstmt& node)
{
  if (checkExpr(node.value) != dataType::int_ && checkExpr(node.value) != dataType::bool_)
  {
    std::cerr<<filename<<": error: exit() expects int or bool, got "<<toStr(checkExpr(node.value))<<std::endl;
    exit(EXIT_FAILURE);
  }
}

void checkStmt(const block* node)
{
  typeScope.push_back({}); // new scope
  for(auto &stmt : node->statements)
  {
    std::visit([](const auto& statement){checkStmt(statement);}, stmt);
  }
  typeScope.pop_back();
}

void typeCheck(const Program& program)
{
  for(auto &stmt : program.statements)
  {
    std::visit([](const auto& statement){checkStmt(statement);}, stmt);
  }
}

void typeError(const token& tok, const std::string& str)
{
  std::cerr<<filename<<":"<<tok.line<<":"<<tok.chr<<": error: "<<str<<std::endl;
  exit(EXIT_FAILURE);
}


dataType tokenToType(TokenType token)
{
  switch (token) {
    case TokenType::bool_:
      return dataType::bool_;
    case TokenType::float_:
      return dataType::float_;
    case TokenType::char_:
      return dataType::char_;
    case TokenType::int_:
      return dataType::int_;
    default:
      exit(EXIT_FAILURE);
  }
}


std::string toStr(dataType type)
{
  switch (type) {
    case dataType::int_:
      return "int";
    case dataType::float_:
      return "float";
    case dataType::char_:
      return "char";
    case dataType::bool_:
      return "bool";
    default:
      exit(EXIT_FAILURE);
  }
  return "UNKNOWN";
}
