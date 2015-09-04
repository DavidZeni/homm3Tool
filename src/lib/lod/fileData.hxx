#pragma once

#include <string>

namespace lod
{

struct H3lod_file {
  char name[16]; // null-terminated, sometimes null-padded too, sometimes padded with, well, something after the null
  uint32_t offset; // includes the header size, no preprocessing required
  uint32_t size_original; // before compression, that is
  uint32_t type; // what's in the file - probably not used by the game directly, more on that below
  uint32_t size_compressed; // how many bytes to read, starting from offset - can be zero for stored files, use size_original in such case 
};

union File {
  H3lod_file o;
  char r[sizeof(H3lod_file)];
};

class DLL_LINK FileData
{
public:
  FileData();
  FileData( const FileData& fileData );
  ~FileData();

private:
  std::string fileName;
  uint32_t offset; // includes the header size, no preprocessing required
  uint32_t size_original; // before compression, that is
  uint32_t type; // what's in the file - probably not used by the game directly, more on that below
  uint32_t size_compressed; // how many bytes to read, starting from offset - can be zero for stored files, use size_original in such case 
};

}