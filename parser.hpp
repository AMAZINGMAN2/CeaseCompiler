#pragma once
#include "lexer.hpp"
#include "codegen.hpp"
#include <cstdlib>
#include <unordered_map>
void expected(std::string str);

std::unordered_map<std::string, int> variables; // the hashmap that stores the variables for O(1) lookup


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


     // =====================
     // PARSE LET
     // =====================

     if(TokenVector.at(i).type == TokenType::let)
     {
       if(TokenVector.at(i+1).type == TokenType::ident_lit)
       {
         if(TokenVector.at(i+2).type == TokenType::eq)
         {
           if(TokenVector.at(i+3).type == TokenType::int_lit)
           {
             variables[TokenVector.at(i+1).value.value()] = std::stoi(TokenVector.at(i+3).value.value());
             // std::cout<<variables["hello"]<<"\n";
           }
         }
       }
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
