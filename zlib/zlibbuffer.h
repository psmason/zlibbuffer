#pragma once

#include <streambuf>
#include <vector>

#include <stdio.h>
#include <zlib.h>

class ZlibBuffer : public std::streambuf
{
private:
  // DATA
  std::vector<unsigned char> d_zinbuffer;
  std::vector<unsigned char> d_zoutbuffer;
  std::vector<char>          d_buffer;
  FILE                      *d_filePtr;
  z_stream                   d_zstream;

public: 
  ZlibBuffer(const std::string & filePath);
  ~ZlibBuffer();

private:
  virtual int underflow();
    // Reads deflated data from the zipped file.
};
