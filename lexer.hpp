#pragma once
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <vector>
enum class TokenType
{
  exit, //exit keyword
  openParen, //open parenthesis `(`
  closeParen,//close parenthesis `)`
  int_lit, // integer literal e.g 2 or 5
};

std::string toStr(TokenType type)
{
  switch (type) {
    case TokenType::exit:
      return "exit";
    case TokenType::openParen:
      return "`(`";
    case TokenType::closeParen:
      return "`)`";
    case TokenType::int_lit:
      return "int literal";
  }
}

struct token // token struct with type because its statically typed
{
  TokenType type;
  std::optional<std::string> value;
};
std::vector<token> TokenVector;
void lex(const std::string& fileContents) // the verb of the word lexer
{
  auto length = fileContents.length();

char c;
std::string currentToken;
// looping over all the characters in the file and tokenizing/lexing it.
for (unsigned int index = 0; index < length; index++) {
  c = fileContents[index];
  if (std::isalpha(fileContents[index])) // if is alphabetical
    {
    currentToken+=c;
    while(index+1 < length && std::isalnum(fileContents[index+1]))
    {
      index++;
      c = fileContents[index];
      currentToken += c;
    }
    if (currentToken == "exit") //exit keyword
    {
      TokenVector.push_back({TokenType::exit, currentToken});
    }
    else {
      std::cerr<<"error at: "<<currentToken;
      exit(EXIT_FAILURE);
    }
  } 
  else if (std::isdigit(c)) {
    currentToken.clear();
    c = fileContents[index];
    currentToken += c;
    while(index+1 < length && std::isdigit(fileContents[index+1]))
    {
      index++;
      c = fileContents[index];
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
  // std::cout<<TokenVector.at(0).value.value()<<std::endl;
  // std::cout<<"lexed"<<std::endl;
}

