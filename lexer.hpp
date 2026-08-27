#pragma once
#include <iostream>
#include <optional>
#include <string>
#include <vector>
void lex(const std::string& fileContents) // the verb of the word lexer
{
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
for (unsigned int index = 0; index < fileContents.length(); index++) {
  c = fileContents.at(index);
  if (std::isalpha(fileContents.at(index))) {
    currentToken+=c;
    while(index + 1 < fileContents.length() && std::isalpha(fileContents.at(index+1)))
    {
      index++;
    c = fileContents.at(index);
      currentToken += c;
    }
    if (currentToken == "exit") 
    {
      TokenVector.push_back({TokenType::exit, currentToken});
    }
  } else if (std::isdigit(c)) {
    currentToken.clear();
    c = fileContents.at(index);
    currentToken += c;
    while(index + 1 < fileContents.length() && std::isdigit(fileContents.at(index+1)))
    {
      index++;
      c = fileContents.at(index);
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
  std::cout<<TokenVector.at(2).value.value()<<std::endl;
  std::cout<<"lexed"<<std::endl;
}

