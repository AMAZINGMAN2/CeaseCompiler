#pragma once
#include "lexer.hpp"
#include <cstdlib>

void expected(std::string str);

// TokenVector
// void parse()
// {
//    for(size_t i = 0; i + 3 < TokenVector.size(); i++)
//    {
//
//      // =====================
//      // PARSE EXIT
//      // =====================
//
//      if(TokenVector.at(i).type == TokenType::exit)
//      {
//        if(TokenVector.at(i+1).type == TokenType::openParen)
//        {
//          if(TokenVector.at(i+2).type == TokenType::ident_lit)
//          {
//            if(TokenVector.at(i+3).type == TokenType::closeParen)
//            {
//              auto name = TokenVector.at(i+2).value.value();
//              auto it = variables.find(name);
//              if(it == variables.end())
//              {
//                std::cerr << "Undefined variable: " << name << "\n";
//                exit(EXIT_FAILURE);
//              }
//              _exit(it->second);
//              i+=3; // probably unneccesary
//            } else{expected(toStr(TokenType::closeParen));}
//          }
//          else if (TokenVector.at(i+2).type == TokenType::int_lit) {
//            if(TokenVector.at(i+3).type == TokenType::closeParen)
//            {
//              _exit(std::stoi(TokenVector.at(i+2).value.value()));
//            } else{expected(toStr(TokenType::closeParen));}
//
//          } else{expected(toStr(TokenType::closeParen));}
//        }else{expected(toStr(TokenType::openParen));}
//        } 
//
//
//      // =====================
//      // PARSE LET
//      // =====================
//
//      if(TokenVector.at(i).type == TokenType::let)
//      {
//        if(TokenVector.at(i+1).type == TokenType::ident_lit)
//        {
//          if(TokenVector.at(i+2).type == TokenType::eq)
//          {
//            if(TokenVector.at(i+3).type == TokenType::int_lit)
//            {
//              variables[TokenVector.at(i+1).value.value()] = std::stoi(TokenVector.at(i+3).value.value());
//              // std::cout<<variables["hello"]<<"\n";
//            }
//          }
//        }
//      }
//
//    }
// }

statement parseStatement();
size_t i = 0;
Program parse() // the main parse function looping over all the tokens from the lexer and using the ast
{
    Program program;
    for(; i < TokenVector.size(); i++)
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
    i+=3;
    return stmt;
  }
  else if (TokenVector.at(i).type == TokenType::let)
  {
    auto stmt = parseLet();
    i+=3;
    return stmt;
  }
  expected("statement");
}

// PARSEING STATEMENTS
// use guard clauses for readability


exitstmt parseExit()
{
  if(TokenVector.at(i+1).type != TokenType::openParen)
  {
    expected(toStr(TokenType::openParen));
  }

  if(TokenVector.at(i+3).type != TokenType::closeParen)
  {
    expected(toStr(TokenType::closeParen));
  }

  if(TokenVector.at(i+2).type == TokenType::ident_lit)
  {
    return exitstmt{identLit{TokenVector.at(i+2)}};
  }
  else if (TokenVector.at(i+2).type == TokenType::int_lit) {

    return exitstmt{intLit{TokenVector.at(i+2)}};
  }
  expected("identifier or literal");
}

letstmt parseLet()
{
  if(TokenVector.at(i+1).type != TokenType::ident_lit)
  {
    expected(toStr(TokenType::ident_lit));
  }
  if(TokenVector.at(i+2).type != TokenType::eq)
  {
    expected(toStr(TokenType::eq));
  }
  if(TokenVector.at(i+3).type != TokenType::int_lit)
  {
    expected(toStr(TokenType::int_lit));
  }
  return letstmt{TokenVector.at(i+1), intLit{TokenVector.at(i+3)}};
}

void expected(std::string str)
{
  std::cerr<<"Expected: "<<str<<std::endl;
  exit(EXIT_FAILURE);
}
