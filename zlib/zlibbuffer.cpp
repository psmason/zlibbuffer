#include <zlibbuffer.h>

#include <iostream>
#include <stdexcept>
#include <cstring>

// See http://www.zlib.net/zlib_how.html

namespace {
  const size_t BUFFER_SIZE = 1000;

  void initializeZStream(z_stream & stream)
  {
    stream.zalloc   = Z_NULL;
    stream.zfree    = Z_NULL;
    stream.opaque   = Z_NULL;
    stream.avail_in = 0;
    stream.next_in  = Z_NULL;
    const int rc    = inflateInit2(&stream, 16+MAX_WBITS);

    if (rc != Z_OK) {
      throw std::runtime_error("Failed to initialize stream");
    }
  }
}  // close anonymous

ZlibBuffer::ZlibBuffer(const std::string & filePath)
  : d_zinbuffer(BUFFER_SIZE)
  , d_zoutbuffer(BUFFER_SIZE)
  , d_buffer(BUFFER_SIZE)
  , d_filePtr(NULL)
{
  std::clog << "Constructing zlib buffer with size "
            << BUFFER_SIZE
            << std::endl;

  d_filePtr = fopen(filePath.c_str(), "r");
  if (!d_filePtr) {
    throw std::runtime_error("Failed to open file " + filePath); 
  }

  initializeZStream(d_zstream);
}

ZlibBuffer::~ZlibBuffer()
{
  if (d_filePtr)
    fclose(d_filePtr);
}

int ZlibBuffer::underflow()
{
  std::clog << "underflow called!" << std::endl;
  if (gptr() < egptr()) {
    throw std::runtime_error("Unexpected underflow call...");
  }

  std::clog << "initial avail_in:"  << d_zstream.avail_in << std::endl;
  std::clog << "initial avail_out:" << d_zstream.avail_out << std::endl;

  if (0 == d_zstream.avail_in) {
    std::clog << "0'ing out old buffer" << std::endl;
    memset((void *) &d_zinbuffer.front()
           , 0
           , BUFFER_SIZE);

    std::clog << "Reading more data for the zlib input buffer" << std::endl;
    d_zstream.avail_in = fread(&d_zinbuffer.front(), 1, BUFFER_SIZE, d_filePtr);

    if (0 == d_zstream.avail_in) {
      std::clog << "Nothing left to read in the zipped file!"
                << std::endl;
      return traits_type::eof();
    }

    std::clog << "avail_in after reading:" << d_zstream.avail_in << std::endl;
    if (ferror(d_filePtr)) {
      inflateEnd(&d_zstream);
      return traits_type::eof();
    }
    d_zstream.next_in = &d_zinbuffer.front();
  }
  else {
    // Continuing where we left off
    //d_zstream.next_in = &d_zinbuffer.front() + BUFFER_SIZE - d_zstream.avail_in;
  }

  d_zstream.avail_out = BUFFER_SIZE;
  d_zstream.next_out  = &d_zoutbuffer.front();

  int rc = inflate(&d_zstream, Z_NO_FLUSH);
  if (Z_STREAM_ERROR == rc) {
    throw std::runtime_error("Z_STREAM_ERROR encountered");
  }
  switch (rc) {
  case Z_NEED_DICT:
    std::clog << "Z_NEED_DICT error" << std::endl;
    rc = Z_DATA_ERROR;     
  case Z_DATA_ERROR:
    std::clog << "Z_DATA_ERROR" << std::endl;
  case Z_MEM_ERROR:
    inflateEnd(&d_zstream);
    std::clog << "Encountered error: " << rc << std::endl;
    return traits_type::eof();
  }
    
  const size_t have = BUFFER_SIZE - d_zstream.avail_out;
  std::clog << "copying " << have << " ..." << std::endl;
  std::memcpy((void*) &d_buffer.front()
              , (void*) &d_zoutbuffer.front()
              , have);

  // Set buffer pointers
  setg(&d_buffer.front()
       , &d_buffer.front()
       , &d_buffer.front() + have);

  std::clog << "avail_in remaining:"  << d_zstream.avail_in << std::endl;
  std::clog << "avail_out remaining:" << d_zstream.avail_out << std::endl;
    
  return traits_type::to_int_type(*gptr());
}
