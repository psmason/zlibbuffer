#include <simplebuffer.h>

#include <stdexcept>
#include <iostream>

SimpleBuffer::SimpleBuffer(const int bufferSize)
  : d_buffer(bufferSize)
  , d_urandom(NULL)
{
  std::cout << "Constructing the simple buffer with size " 
            << bufferSize
            << std::endl;

  d_urandom = fopen("/dev/urandom", "r");
  if (NULL == d_urandom) {
    std::cout << "Failed to open /dev/urandom" << std::endl;
    throw std::runtime_error("/dev/urandom error");
  }

  char *end = &d_buffer.front() + d_buffer.size();
  setg(end, end, end);
}

int SimpleBuffer::underflow()
{
  std::cout << "underflow called!" << std::endl;
  if (gptr() < egptr()) {
    std::cout << "... but underflow not needed?" << std::endl;
  }

  size_t n = ::fread(&d_buffer.front(), 1, d_buffer.size(), d_urandom);
  if (0 == n) {
    std::cout << "no more data to read? this shouldn't happen with /dev/urandom" 
              << std::endl;
    return traits_type::eof();
  }

  std::cout << "read " << n << " from /dev/urandom" << std::endl;

    // Set buffer pointers
  setg(&d_buffer.front()
       , &d_buffer.front()
       , &d_buffer.front() + n);

  return traits_type::to_int_type(*gptr());
}
                
