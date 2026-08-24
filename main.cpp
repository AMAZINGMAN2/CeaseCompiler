#include <iostream>
#include <string>
#include "lexer.hpp"
int main (int argc, char *argv[]) {
  if(argc != 2)
  {
    std::cout<<"Usage: cease <FileName>.cz"<<std::endl;
  }
  else{
  std::cout<<"running: "<<argv[1]<<std::endl;
  }
  lex(argv[1]);
  //tokenizing the text inside the file into a vector for now, might change to hashmap or ordered_map or somthing later
  return 0;
}
