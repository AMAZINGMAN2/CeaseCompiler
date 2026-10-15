#pragma once
#include "lexer.hpp"
#include <cstdlib>
void expected(std::string str);
void expected(TokenType expectedType, token currToken);
token expect(TokenType type);
Expr parseExpr(int minPrec = 0);
statement parseStatement();
size_t i = 0;
std::string filename;
Program parse(std::string file) // the main parse function looping over all the tokens from the lexer and using the ast
{
  filename = file;
    Program program;
    while(i < TokenVector.size())
    {
      program.statements.push_back(parseStatement());
    }
    return program;
}
assignstmt parseAssign();
block parseBlock();
exitstmt parseExit();
letstmt parseLet();
statement parseStatement()
{
  if (TokenVector.at(i).type == TokenType::exit)
  {
    return parseExit();
  }
  if (TokenVector.at(i).type == TokenType::openBrace)
  {
    return parseBlock();
  }
  else if (TokenVector.at(i).type == TokenType::let)
  {
    return parseLet();
  }
  else if (TokenVector.at(i).type == TokenType::ident_lit)
  {
    return parseAssign();
  }
  expected("statement");
  exit(EXIT_FAILURE);
}


// PARSEING STATEMENTS
// use guard clauses for readability

Expr computeAtom()
{
  if(i >= TokenVector.size())
  {
    expected("expression");
  }
  token value = TokenVector.at(i++);
  if(value.type == TokenType::ident_lit)
  {
    return identLit{value};
  }
  else if(value.type == TokenType::int_lit)
  {
    return intLit{value};
  }
  else if (value.type == TokenType::openParen)
  {
    Expr result = parseExpr();
    expect(TokenType::closeParen);
    return result;
  }
  else if(value.type == TokenType::minus)
  {
    Expr result = computeAtom();
    return new unExpr{value, result};
  }
  expected("statement");
  exit(EXIT_FAILURE);
}


Expr parseExpr(int minPrec)
{
  Expr result = computeAtom();
  while(i < TokenVector.size() && prec(TokenVector.at(i).type) >= minPrec)
  {
    int curPred = prec(TokenVector.at(i).type);
    token op = TokenVector.at((i++));
    Expr rhs = parseExpr(curPred+1);
    result = new binExpr{result, op , rhs};
  }
  return result;
}



exitstmt parseExit()
{
  expect(TokenType::exit);
  expect(TokenType::openParen);
  auto value = parseExpr();
  expect(TokenType::closeParen);
  return exitstmt{value};
}

letstmt parseLet()
{
  expect(TokenType::let);
  token name = expect(TokenType::ident_lit);
  expect(TokenType::eq);
  auto value = parseExpr();
  return letstmt{name, value};
}

block parseBlock()
{
  expect(TokenType::openBrace);
  block result;
  while(i < TokenVector.size() && TokenVector.at(i).type != TokenType::closeBrace)
  {
    result.statements.push_back(parseStatement());
  }
  expect(TokenType::closeBrace);
  return result;
}


assignstmt parseAssign()
{
  token name = expect(TokenType::ident_lit);
  expect(TokenType::eq);
  return assignstmt{name, parseExpr()};
}


token expect(TokenType type)
{
  if(i >= TokenVector.size())
  {
    std::cerr<<filename<<": error:  Expected "<<toStr(type)<<"got EOF"<<std::endl;
    exit(EXIT_FAILURE);
  }
  if(TokenVector.at(i).type != type)
  {
    expected(type, TokenVector.at(i));
  }
  return TokenVector.at(i++); //increments i after returning it
}


void expected(TokenType expectedType, token currToken)
{
  std::cerr<<filename<<":"<<currToken.line<<":"<<currToken.chr<<": error: Expected "<<
    toStr(expectedType)<<" got "<<toStr(currToken.type)<<std::endl;
  exit(EXIT_FAILURE);
}
void expected(std::string str)
{
  std::cerr<<filename<<":"<<TokenVector.at(i-1).line<<":"<<TokenVector.at(i-1).chr<<": error: Expected "<<str<<std::endl;
  exit(EXIT_FAILURE);
}
