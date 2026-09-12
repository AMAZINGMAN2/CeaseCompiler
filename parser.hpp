#pragma once
#include "lexer.hpp"
#include <cstdlib>

void expected(std::string str);
token expect(TokenType type);

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

Expr parseExpr()
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
  expected("identifier or literal");
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
