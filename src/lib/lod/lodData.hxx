#pragma once

#include <vector>

namespace lod
{

struct H3lod {
	uint32_t magic; // always 0x00444f4c, that is, a null-terminated "LOD" string
	uint32_t type; // 200 for base archives, 500 for expansion pack archives, probably completely arbitrary numbers
	uint32_t files_count; // obvious
	uint8_t unknown[80]; // 80 bytes of hell knows what - in most cases that's all zeros, but in H3sprite.lod there's a bunch of seemingly irrelevant numbers here, any information is welcome
}; 
	//struct H3lod_file[10000]; // file list

union Header {
	H3lod o;
	char r[sizeof(H3lod)];
};

class DLL_LINK LodData
{
public:
  lodData();
  lodData( const LodData& lodData );
  ~lodData();

private:
  uint32_t _fileCount;
  std::vector<> _files;
};

}