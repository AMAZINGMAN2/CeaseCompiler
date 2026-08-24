#pragma once
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cwctype>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
void lex(std::string fileName) // the verb of the word lexer
{
  std::ifstream file(fileName);
  if(!file) // checking if file is valid
  {
    std::cout<<"unable to open file "<<fileName;
    exit(EXIT_FAILURE);
  }
  enum class TokenType
  {
    exit, //exit keyword
    openParen, //open parenthesis `(`
    closeParen,//close parenthesis `)`
    int_lit, // integer literal e.g 2 or 5
  };

  struct token // token struct with type because its statically typed
  {
    TokenType type;
    std::optional<std::string> value;
  };
  std::vector<token> TokenVector;

char c;
std::string currentToken;
// looping over all the characters in the file and tokenizing/lexing it.
while (file.get(c)) {
  if (std::isalpha(c)) {
    currentToken+=c;
    while(file.peek() && std::isalpha(file.peek()))
    {
      file.get(c);
      currentToken += c;
    }
    if (currentToken == "exit") 
    {
      TokenVector.push_back({TokenType::exit, currentToken});
    }
  } else if (std::isdigit(c)) {
    currentToken.clear();
    currentToken += c;
    while(file.peek() && std::isdigit(file.peek()))
    {
      file.get(c);
      currentToken += c;
    }

    TokenVector.push_back({TokenType::int_lit, currentToken});
  } else if (c == '(') {
    TokenVector.push_back({TokenType::openParen});
  } else if (c == ')') {
    TokenVector.push_back({TokenType::closeParen});
  } else if (std::iswspace(c)) {
    continue;
  } else {
    std::cerr<<"Syntax Error"<<std::endl;
    exit(EXIT_FAILURE);
  }
}
  file.close();
  std::cout<<TokenVector.at(2).value.value()<<std::endl;
  std::cout<<"lexed"<<std::endl;
}
