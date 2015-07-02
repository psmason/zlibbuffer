#include <vector>
#include <streambuf>

#include <stdio.h>

class SimpleBuffer : public std::streambuf 
{
private:
  // DATA
  std::vector<char> d_buffer;
  FILE            *d_urandom;

public:
  SimpleBuffer(const int bufferSize = 100);
    // Size of the internal buffer.

private:
  virtual int underflow();
    // Reads from /dev/urandom to fill in the buffer.
};
