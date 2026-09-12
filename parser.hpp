#pragma once
#include "lexer.hpp"
#include <cstdlib>

void expected(std::string str);
token expect(TokenType type);
Expr parseExpr(int minPrec = 0);
statement parseStatement();
size_t i = 0;
Program parse() // the main parse function looping over all the tokens from the lexer and using the ast
{
    Program program;
    while(i < TokenVector.size())
    {
      program.statements.push_back(parseStatement());
    }
    return program;
}


exitstmt parseExit();
letstmt parseLet();
statement parseStatement()
{
  if (TokenVector.at(i).type == TokenType::exit)
  {
    auto stmt = parseExit();
    return stmt;
  }
  else if (TokenVector.at(i).type == TokenType::let)
  {
    auto stmt = parseLet();
    return stmt;
  }
  expected("statement");
}


// PARSEING STATEMENTS
// use guard clauses for readability

Expr computeAtom()
{
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
  expected("identifier or literal");

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


token expect(TokenType type)
{
  if(i >= TokenVector.size() || TokenVector.at(i).type != type)
  {
    expected(toStr(type));
  }
  return TokenVector.at(i++); //increments i after returning it
}


void expected(std::string str)
{
  std::cerr<<"Expected: "<<str<<std::endl;
  exit(EXIT_FAILURE);
}
