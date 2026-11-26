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


struct type
{
  dataType base;
  int depth;
  bool operator==(const type& o) const {return base == o.base && depth == o.depth;}
  bool operator!=(const type& o) const {return !(*this == o);}
};

std::string toStr(dataType t);
std::string toStr(type t);
dataType tokenToType(TokenType token);
void typeError(const token& tok, const std::string& str);
std::vector<std::unordered_map<std::string, type>> typeScope(1);

type checkExpr(const identLit& node);
type checkExpr(const unExpr *node);
type checkExpr(const binExpr& node);
type checkExpr(const binExpr *node);
type checkExpr(const Expr& expr);
void checkStmt(const block* node);
void checkStmt(const forstmt& node);
void checkStmt(const whilestmt& node);
void checkStmt(const assignstmt& node);
void checkStmt(const letstmt& node);
void checkStmt(const ifstmt& node);

type checkExpr(const intLit&){return {dataType::int_,0};}
type checkExpr(const floatLit&){return {dataType::float_,0};}
type checkExpr(const boolLit&){return {dataType::bool_,0};}
type checkExpr(const charLit&){return {dataType::char_,0};}
type checkExpr(const identLit& node)
{
  for(auto i = typeScope.rbegin(); i != typeScope.rend(); i++)
  {
    auto found = i->find(node.name.value.value());
    if(found != i->end())
    {return found->second;}
  }
  typeError(node.name, "Undefined variable '" + node.name.value.value() + "'");
  return {dataType::int_, 0};
}

type checkExpr(const unExpr&  node)
{
  if (node.op.type == TokenType::minus)
  {
    type inner = checkExpr(node.value);
    if(inner.depth != 0 || (inner.base != dataType::int_ && inner.base != dataType::float_))
    {
    typeError(node.op, "Unary '-' requires int or float, got " + toStr(inner.base));
    }
    return inner;
  }
  else if(node.op.type == TokenType::amp)
  {
    if (!std::holds_alternative<identLit>(node.value) && !(std::holds_alternative<unExpr*>(node.value) && std::get<unExpr*>(node.value)->op.type == TokenType::star))
      {
      typeError(node.op,"Cannot take address of rvalue");
    }
    type inner = checkExpr(node.value);
    return {inner.base, inner.depth + 1};
  }
  else if(node.op.type == TokenType::star)
  {
    type inner = checkExpr(node.value);
    if(inner.depth == 0)
    {
      typeError(node.op ,"Cannot dereference non pointer \'" + toStr(inner.base) + "\'");
    }
    return {inner.base, inner.depth - 1};
  }
  typeError(node.op, "Unsupported unary operator");
  return {dataType::int_,0};
}



type checkExpr(const unExpr *node)
{
  return checkExpr(*node);
}




type checkExpr(const binExpr& node)
{
  type left = checkExpr(node.left);
  type right = checkExpr(node.right);
  TokenType op = node.op.type;

if (op == TokenType::plus || op == TokenType::minus || op == TokenType::star || op == TokenType::fslash)
{
  if (left != right || left.depth != 0 ||(left.base != dataType::int_ && left.base != dataType::float_))
  {
    typeError(node.op, "Invalid arthmetic" + toStr(left.base) + " and " + toStr(right.base));
  }
  return left;
}

if (op == TokenType::greaterthan || op == TokenType::smallerthan || op == TokenType::greaterEq || op == TokenType::smallerEq)
{
  if (left != right || left.depth != 0 || left.base == dataType::bool_)
  {
    typeError(node.op, "Cannot compare" + toStr(left.base) + " and " + toStr(right.base));
  }
  return {dataType::bool_,0};
}


if (op == TokenType::eqEq || op == TokenType::notEq)
{
  if (left != right)
  {
    typeError(node.op, "Cannot compare" + toStr(left.base) + " and " + toStr(right.base));
  }
  return {dataType::bool_,0};
}

typeError(node.op, "Unknown binary operator");
  return {dataType::int_,0};
}


type checkExpr(const binExpr *node)
{
  return checkExpr(*node);
}



type checkExpr(const Expr& expr)
{
  return std::visit([](const auto& node){return checkExpr(node);}, expr);
}

void checkStmt(const block* node);

void checkStmt(const letstmt& node)
{
  type declared{tokenToType(node.type.base), node.type.depth};
  if(declared != checkExpr(node.value))
  {
    typeError(node.name, "Type mismatch of \'"+ node.name.value.value()+"\'");
  }
  if(!typeScope.back().emplace(node.name.value.value(), declared).second)
  {
    typeError(node.name, "Redeclaration of \'" + node.name.value.value() + "\'");
  }
}

void checkStmt(const deref& node)
{
  if(checkExpr(node.target) != checkExpr(node.value))
  {
    typeError(TokenVector.at(0), "Type mismatch in pointer assignment");
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
        typeError(node.name,"Cannot assign \'" + toStr(checkExpr(node.value).base) + "\'");
      }
      return;
    }
  }
  typeError(node.name, "Undefined variable '" + node.name.value.value() + "'");
}

void checkStmt(const ifstmt& node)
{
  for (const auto& branch : node.branches) {
    if(checkExpr(branch.cond) != type{dataType::bool_,0})
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
    if(checkExpr(node.cond) != type{dataType::bool_,0})
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
  if(checkExpr(node.cond) != type{dataType::bool_,0})
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
  type val = checkExpr(node.value);
  if (val != type{dataType::int_, 0} && val != type{dataType::bool_,0})
  {
    std::cerr<<filename<<": error: exit() expects int or bool, got "<<toStr(checkExpr(node.value).base)<<std::endl;
    exit(EXIT_FAILURE);
  }
}


void checkStmt(const printstmt& node)
{
  type val = checkExpr(node.value);
  if (val != type{dataType::int_, 0} && val != type{dataType::bool_,0} && val != type{dataType::float_,0} && val != type{dataType::char_,0})
  {
    std::cerr<<filename<<": error: print() expects int or bool, float or address, got "<<toStr(checkExpr(node.value).base)<<std::endl;
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


std::string toStr(dataType t)
{
  switch (t) {
    case dataType::int_:
      return "int";
    case dataType::float_:
      return "float";
    case dataType::char_:
      return "char";
    case dataType::bool_:
      return "bool";
  }
  return "UNKNOWN";
}



std::string toStr(type t)
{
  return toStr(t.base) + std::string(t.depth,'*');
}
