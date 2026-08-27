#pragma once
#include "lexer.hpp"
#include "codegen.hpp"
#include <cstdlib>
void expected(std::string str);

// struct intLit
// {
//   token int_lit;
// };


// TokenVector
void parse()
{
   for(size_t i = 0; i + 3 < TokenVector.size(); i++)
   {
     // =====================
     // PARSE EXIT
     // =====================
     if(TokenVector.at(i).type == TokenType::exit)
     {
       if(TokenVector.at(i+1).type == TokenType::openParen)
       {
         if(TokenVector.at(i+2).type == TokenType::int_lit)
         {
           if(TokenVector.at(i+3).type == TokenType::closeParen)
           {
             _exit(std::stoi(TokenVector.at(i+2).value.value())); // exits with the integer after the parenthesis
             i+=3; // probably unneccesary
           } else{expected(toStr(TokenType::closeParen));}
         } else{expected(toStr(TokenType::int_lit));}

       } else{expected(toStr(TokenType::openParen));}
     }
     // ADD NEXT FEATURES HERE. THE BELOW EXIT ISNT SUPPOSED TO BE REACHED
     else {
       exit(EXIT_FAILURE);
     }
   }
}


void expected(std::string str)
{
  std::cerr<<"Expected: "<<str;
  exit(EXIT_FAILURE);
}
