#pragma once
#include "lexer.hpp"
#include <cstdlib>
void expected(std::string str);
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
block* parseBlock();
exitstmt parseExit();
letstmt parseLet();
ifstmt parseIf();
whilestmt parseWhile();
forstmt parseFor();
void expected(TokenType expectedType, token currToken);
token expect(TokenType type);
bool isTypeToken(TokenType type);
Expr computeAtom();


statement parseStatement()
{
  if (TokenVector.at(i).type == TokenType::for_)
  {
    return parseFor();
  }
  if (TokenVector.at(i).type == TokenType::while_)
  {
    return parseWhile();
  }
  if (TokenVector.at(i).type == TokenType::if_)
  {
    return parseIf();
  }
  if (TokenVector.at(i).type == TokenType::exit)
  {
    auto stmt = parseExit();
    expect(TokenType::semi);
    return stmt;
  }
  if (TokenVector.at(i).type == TokenType::openBrace)
  {
    return parseBlock();
  }
  else if (TokenVector.at(i).type == TokenType::ident_lit)
  {
    auto stmt = parseAssign();
    expect(TokenType::semi);
    return stmt;
  }
  else if(isTypeToken(TokenVector.at(i).type))
  {
    auto stmt = parseLet();
    expect(TokenType::semi);
    return stmt;
  }
  else if (TokenVector.at(i).type == TokenType::star)
  {
    Expr target = computeAtom();
    expect(TokenType::eq);
    auto stmt = deref{target, parseExpr()};
    expect(TokenType::semi);
    return stmt;
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
  else if(value.type == TokenType::bool_lit)
  {
    return boolLit{value};
  }
  else if(value.type == TokenType::float_lit)
  {
    return floatLit{value};
  }
  else if(value.type == TokenType::char_lit)
  {
    return charLit{value};
  }
  else if (value.type == TokenType::openParen)
  {
    Expr result = parseExpr();
    expect(TokenType::closeParen);
    return result;
  }
  else if(value.type == TokenType::minus || value.type == TokenType::star || value.type == TokenType::amp)
  {
    return new unExpr{value, computeAtom()};
  }
  expected("expression");
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

  if(i < TokenVector.size() && !isTypeToken(TokenVector.at(i).type))
  {
    expected("type");
  }
  typeInfo type{TokenVector.at(i++).type, 0 }; // initialise depth as 0
  while(i < TokenVector.size() && TokenVector.at(i).type == TokenType::star)
  {
    type.depth++; i++; //if type* increase depth and go to next token/char
  }
  // if(TokenVector.at(i).type != TokenType::int_ && TokenVector.at(i).type != TokenType::float_ && TokenVector.at(i).type != TokenType::char_ && TokenVector.at(i).type != TokenType::bool_)
  // {
  //   expected("initializer");
  // }
  // i++;
  // token name = expect(TokenType::ident_lit);
  // expect(TokenType::eq);
  // auto value = parseExpr();
  // TokenType type = TokenVector.at(i++).type;
  token name = expect(TokenType::ident_lit);
  expect(TokenType::eq);
  return letstmt{type, name, parseExpr()};
}

block* parseBlock()
{
  expect(TokenType::openBrace);
  // block result;
  auto* result = new block;
  while(i < TokenVector.size() && TokenVector.at(i).type != TokenType::closeBrace)
  {
    result->statements.push_back(parseStatement());
  }
  expect(TokenType::closeBrace);
  return result;
}

// this is for if, while etc etc. very informative
// craftinginterpreters.com/control-flow.html

ifstmt parseIf(){
  ifstmt result; // has branches (vector) and elsebody (block*)
  expect(TokenType::if_);
  expect(TokenType::openParen);
  Expr cond = parseExpr();
  expect(TokenType::closeParen);

  result.branches.push_back({cond, parseBlock()});
  while(i < TokenVector.size() && TokenVector.at(i).type == TokenType::elseif_)
  {
  expect(TokenType::elseif_);
    expect(TokenType::openParen);
    Expr cond = parseExpr();
    expect(TokenType::closeParen);
    result.branches.push_back({cond, parseBlock()});
  }
  if(i < TokenVector.size() && TokenVector.at(i).type == TokenType::else_)
  {
  expect(TokenType::else_);
    result.elsebody = parseBlock();
  }
  return result;
}

whilestmt parseWhile(){
  expect(TokenType::while_);
  expect(TokenType::openParen);
  Expr cond = parseExpr();
  expect(TokenType::closeParen);
  return whilestmt{cond, parseBlock()};
}


forstmt parseFor(){
  expect(TokenType::for_);
  expect(TokenType::openParen);
  letstmt init = parseLet();
  expect(TokenType::semi);
  Expr cond = parseExpr();
  expect(TokenType::semi);
  assignstmt update = parseAssign();
  expect(TokenType::closeParen);
  return forstmt{init, cond, update, parseBlock()};
}



assignstmt parseAssign()
{
  token name = expect(TokenType::ident_lit);
  expect(TokenType::eq);
  return assignstmt{name, parseExpr()};
}


bool isTypeToken(TokenType type)
{
  return type == TokenType::int_ || type == TokenType::float_ || type == TokenType::bool_  || type == TokenType::char_;
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
  std::cerr<<filename<<":"<<TokenVector.at(i>0? i-1:0).line<<":"<<TokenVector.at(i>0? i-1:0).chr<<": error: Expected "<<str<<std::endl;
  exit(EXIT_FAILURE);
}
