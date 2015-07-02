#include <iostream>
#include <simplebuffer.h>
#include <sstream>
#include <vector>

int main()
{
  const int bufferSize = 30*8;
  SimpleBuffer buffer(bufferSize);

  for (int i=0; i<30; ++i) 
    std::cout << i << ": " << buffer.sbumpc() << std::endl;

  std::istream is(&buffer);

  std::vector<size_t> ints(30);
  is.read((char*)&ints.front(), 30 * sizeof(size_t));

  for (int i=0; i<30; ++i) {
    std::cout << i << ": " << ints[i] % 100 << std::endl;
  }
}
