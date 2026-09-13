#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "codegen.hpp"
#include "lexer.hpp"
#include "parser.hpp"
int main (int argc, char *argv[]) {
  if(argc != 2)
  {
    std::cout<<"Usage: cease <FileName>.cz"<<std::endl;
    exit(EXIT_FAILURE);
  }
  else{
  // std::cout<<"running: "<<argv[1]<<std::endl;
  }
  std::ifstream file(argv[1]);
  if(!file) // checking if file is valid
  {
    std::cout<<"unable to open file "<<argv[1]<<std::endl;
    exit(EXIT_FAILURE);
  }
  std::string fileContents;
  std::string line;
  while(std::getline(file, line))
  {
    fileContents += line + "\n";
  }
  file.close();
  //putting the file into a string, may cause files to only have a size of unsigned int64?? but thats probably more than enough.
  lex(fileContents);
  generator(parse((std::string)argv[1]));
  //tokenizing the text inside the file into a vector for now, might change to hashmap or ordered_map or somthing later
  return 0;
}
