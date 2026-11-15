#pragma once
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <new>
#include <optional>
#include <string>
#include <variant>
#include <vector>


enum class TokenType
{
  exit, //exit keyword
  openParen, //open parenthesis `(`
  closeParen,//close parenthesis `)`
  int_lit, // integer literal e.g 2 or 5
  let, // the let keyword
  ident_lit, // identifier literals. e.g foo or bar
  eq, // =
  plus,
  minus,
  fslash,
  star,
  openBrace,
  closeBrace,
  greaterthan,
  smallerthan,
  eqEq,
  smallerEq,
  greaterEq,
  notEq,
  bool_lit,
  if_,
  else_,
  elseif_,
  while_,
  for_,
  semi,
};

int prec(TokenType type)
{
  switch (type) {
    case TokenType::plus:
      return 2;
    case TokenType::minus:
      return 2;
    case TokenType::fslash:
      return 3;
    case TokenType::star:
      return 3;
    case TokenType::greaterEq:
      return 1;
    case TokenType::smallerEq:
      return 1;
    case TokenType::greaterthan:
      return 1;
    case TokenType::smallerthan:
      return 1;
    case TokenType::eqEq:
      return 0;
    case TokenType::notEq:
      return 0;
    default:
      return -1;
  }
}

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
      return "Int literal";
    case TokenType::let:
      return "let";
    case TokenType::ident_lit:
      return "Identifier";
    case TokenType::eq:
      return "`=`";
    case TokenType::plus:
      return "`+`";
    case TokenType::minus:
      return "`-`";
    case TokenType::fslash:
      return "`/`";
    case TokenType::star:
      return "`*`";
    case TokenType::openBrace:
      return "`{`";
    case TokenType::closeBrace:
      return "`}`";
    case TokenType::greaterthan:
      return "`>`";
    case TokenType::smallerthan:
      return "`<`";
    case TokenType::eqEq:
      return "`==`";
    case TokenType::notEq:
      return "`!=`";
    case TokenType::greaterEq:
      return "`>=`";
    case TokenType::smallerEq:
      return "`<=`";
    case TokenType::bool_lit:
      return "Boolean Expression";
    case TokenType::if_:
      return "`if`";
    case TokenType::else_:
      return "`else`";
    case TokenType::elseif_:
      return "`elseif`";
    case TokenType::while_:
      return "`while`";
    case TokenType::for_:
      return "for";
    case TokenType::semi:
      return "`;`";
  }
}
// A GOOD EXAMPLE OF ABSTRACT SYNTAX TREES. although this implementation differs a little bit
// https://www.slideserve.com/nami/levels-of-programming-languages


struct token // token struct with type because the programming language is statically typed
{
  TokenType type;
  std::optional<std::string> value;
  size_t line;
  size_t chr;
};



struct identLit
{
  token name;
};

struct intLit
{
  token value;
};


struct boolLit
{
  token value;
};


struct binExpr; // binary expression eg. 1 (+) 2
struct unExpr; // unary expression eg. (-)3

using Expr = std::variant<intLit, identLit, binExpr*, unExpr*, boolLit>; // AN EXPRESION IS EITHER AN INTEGER LITERAL OR AN IDENTIFIER

//https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing
// this is for PRECEDENCE CLIMBING


struct unExpr
{
  token op;
  Expr value;
};


struct binExpr
{
  Expr left;
  token op;
  Expr right;
};

struct letstmt
{
  token name;
  Expr value;
};

struct exitstmt
{
  Expr value;
};


struct assignstmt
{
  token name;
  Expr value;
};

struct block;


struct ifbranch
{
  Expr cond;
  block* body;
};

struct ifstmt
{
  std::vector<ifbranch> branches;
  block* elsebody = nullptr;
};


struct whilestmt
{
  Expr cond;
  block* body;
};

struct forstmt
{
  letstmt init;
  Expr cond;
  assignstmt update;
  block* body;
};

// add all possible statement here
using statement = std::variant<letstmt, exitstmt, block*, assignstmt, ifstmt, whilestmt, forstmt>;

struct Program{ // a program is a list of statements.
  std::vector<statement> statements;
};

struct block{ // a program is a list of statements.
  std::vector<statement> statements;
};




std::vector<token> TokenVector;
void lex(const std::string& fileContents) // the verb of the word lexer
{
  auto length = fileContents.length();
  size_t line = 1;
  size_t chr = 1;

char c;
std::string currentToken;
// looping over all the characters in the file and tokenizing/lexing it.
for (unsigned int index = 0; index < length; index++) {
  c = fileContents[index];
  if (std::isalpha(fileContents[index])) // if is alphabetical
    {
      size_t tokenline = line;
      size_t tokenchr = chr;
      currentToken.clear();
      currentToken+=c;
      chr++;
    // loop while the token is alphanumeric
    while(index+1 < length && std::isalnum(fileContents[index+1]))
    {
      index++;
      c = fileContents[index];
      currentToken += c;
      chr++;
    }
    if (currentToken == "exit") //exit keyword
    {
      TokenVector.push_back({TokenType::exit, std::nullopt, tokenline, tokenchr});
    }
    else if (currentToken == "false") //exit keyword
    {
      TokenVector.push_back({TokenType::bool_lit, currentToken, tokenline, tokenchr});
    }
    else if (currentToken == "true") //exit keyword
    {
      TokenVector.push_back({TokenType::bool_lit, currentToken, tokenline, tokenchr});
    }
    else if (currentToken == "let") //exit keyword
    {
      TokenVector.push_back({TokenType::let, std::nullopt, tokenline, tokenchr});
    }
    else if (currentToken == "if")
      TokenVector.push_back({TokenType::if_, std::nullopt, tokenline, tokenchr});
    else if (currentToken == "else")
      TokenVector.push_back({TokenType::else_, std::nullopt, tokenline, tokenchr});
    else if (currentToken == "elseif")
      TokenVector.push_back({TokenType::elseif_, std::nullopt, tokenline, tokenchr});
    else if (currentToken == "while")
      TokenVector.push_back({TokenType::while_, std::nullopt, tokenline, tokenchr});
    else if (currentToken == "for")
      TokenVector.push_back({TokenType::for_, std::nullopt, tokenline, tokenchr});
    else { // add IF for any characters that cannot be accepted in indentifiers. or any exceptions
      TokenVector.push_back({TokenType::ident_lit, currentToken, tokenline, tokenchr});
    }
  } else if (c == '/' && index + 1 < length && fileContents[index + 1] == '/') {
    while(index < length && fileContents[index] != '\n')
    {
    index++;
    // chr++;
    }
  }
  else if (std::isdigit(c)) {
    size_t tokenline = line;
    size_t tokenchr = chr;
    currentToken.clear();
    c = fileContents[index];
    currentToken += c;
    chr++;
    while(index+1 < length && std::isdigit(fileContents[index+1]))
    {
      index++;
      c = fileContents[index];
      currentToken += c;
      chr++;
    }
    TokenVector.push_back({TokenType::int_lit, currentToken, tokenline, tokenchr});
  } else if (c == '(') {
    TokenVector.push_back({TokenType::openParen, std::nullopt, line, chr});
    chr++;
  } else if (c == '+') {
    TokenVector.push_back({TokenType::plus, std::nullopt, line, chr});
    chr++;
  } else if (c == '-') {
    TokenVector.push_back({TokenType::minus, std::nullopt, line, chr});
    chr++;
  } else if (c == '/') {
    TokenVector.push_back({TokenType::fslash, std::nullopt, line, chr});
    chr++;
  } else if (c == '*') {
    TokenVector.push_back({TokenType::star, std::nullopt, line, chr});
    chr++;
  } else if (c == ';') {
    TokenVector.push_back({TokenType::semi, std::nullopt, line, chr});
    chr++;
  }else if (c == '{') {
    TokenVector.push_back({TokenType::openBrace, std::nullopt, line, chr});
    chr++;
  } else if (c == '}') {
    TokenVector.push_back({TokenType::closeBrace, std::nullopt, line, chr});
    chr++;
  } else if (c == ')') {
    TokenVector.push_back({TokenType::closeParen, std::nullopt, line, chr});
    chr++;
  } 
  else if (c == '=') {
    if(index + 1 < length && fileContents[index+1] == '=')
    {
      TokenVector.push_back({TokenType::eqEq, std::nullopt, line, chr});
        index++; chr+=2;
    }
    else{
      TokenVector.push_back({TokenType::eq, std::nullopt, line, chr});
      chr++;
    }
  } 
  else if (c == '!') {
    if(index + 1 < length && fileContents[index+1] == '=')
    {
      TokenVector.push_back({TokenType::notEq, std::nullopt, line, chr});
        index++; chr+=2;
    }
    else{
      exit(EXIT_FAILURE);
      TokenVector.push_back({TokenType::eq, std::nullopt, line, chr});
      chr++;
    }
  } 
  else if (c == '<') {
    if(index + 1 < length && fileContents[index+1] == '=')
    {
      TokenVector.push_back({TokenType::smallerEq, std::nullopt, line, chr});
        index++; chr+=2;
    }
    else{
      TokenVector.push_back({TokenType::smallerthan, std::nullopt, line, chr});
      chr++;
    }
  } 
  else if (c == '>') {
    if(index + 1 < length && fileContents[index+1] == '=')
    {
      TokenVector.push_back({TokenType::greaterEq, std::nullopt, line, chr});
        index++; chr+=2;
    }
    else{
      TokenVector.push_back({TokenType::greaterthan, std::nullopt, line, chr});
      chr++;
    }
  } 
  else if (c == '\n') {
    line++;
    chr = 1;
  }  else if (c == '\r') {
    continue;
  }else if (std::isspace(c)) {
    chr++;
  } else if (std::iswspace(c)) {
    continue;
  }else {
    std::cerr<<"Syntax Error"<<std::endl;
    exit(EXIT_FAILURE);
  }
}
  // std::cout<<toStr(TokenVector.at(0).type)<<std::endl;
  // std::cout<<TokenVector.at(2).value.value()<<std::endl;
  // std::cout<<"lexed"<<std::endl;
}

