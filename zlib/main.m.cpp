#include <zlibbuffer.h>

#include <iostream>
#include <stdlib.h> 

int main(int argc, char * argv[])
{
  if (argc < 2) {
    std::cout << "Please enter a file name for testing" 
              << std::endl;
    exit(1);
  }

  const std::string filePath(argv[1]);
  std::clog << "Running against file "
            << filePath
            << std::endl;

  ZlibBuffer buffer(filePath);

  std::istream is(&buffer);
  std::cout << is.rdbuf();
}
