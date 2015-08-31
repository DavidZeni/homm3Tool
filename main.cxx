#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "zlib.h"

#include "SDL.h"
#include "SDL_image.h"

//#include <boost/filesystem.hpp>
//#include <boost/program_options.hpp>

struct H3lod {
	uint32_t magic; // always 0x00444f4c, that is, a null-terminated "LOD" string
	uint32_t type; // 200 for base archives, 500 for expansion pack archives, probably completely arbitrary numbers
	uint32_t files_count; // obvious
	uint8_t unknown[80]; // 80 bytes of hell knows what - in most cases that's all zeros, but in H3sprite.lod there's a bunch of seemingly irrelevant numbers here, any information is welcome
}; 
	//struct H3lod_file[10000]; // file list

struct H3lod_file {
	char name[16]; // null-terminated, sometimes null-padded too, sometimes padded with, well, something after the null
	uint32_t offset; // includes the header size, no preprocessing required
	uint32_t size_original; // before compression, that is
	uint32_t type; // what's in the file - probably not used by the game directly, more on that below
	uint32_t size_compressed; // how many bytes to read, starting from offset - can be zero for stored files, use size_original in such case 
};

union Header {
	H3lod o;
	char r[sizeof(H3lod)];
};

union File {
	H3lod_file o;
	char r[sizeof(H3lod_file)];
};

bool isPCX( char* buffer, uint32_t bufferSize )
{
	if(bufferSize < (uint32_t)12)
	{
		return false;
	}

	uint32_t* bitmap_size = (uint32_t*)buffer;
	uint32_t* width = (uint32_t*)(buffer+4);
	uint32_t* height = (uint32_t*)(buffer+8);

	return (*bitmap_size) == (*width)*(*height) || (*bitmap_size) == (*width)*(*height)*3;
}

bool convertToPNG( char* buffer, uint32_t bufferSize, const char* name )
{
	uint32_t* bitmap_size = (uint32_t*)buffer;
	uint32_t* width = (uint32_t*)(buffer+4);
	uint32_t* height = (uint32_t*)(buffer+8);

	SDL_RWops* rw = nullptr;

	rw = SDL_RWFromMem(buffer+12, (*bitmap_size));

	SDL_Surface *temp = IMG_Load_RW(rw, 1);

	if((*bitmap_size) == (*width)*(*height))
	{
		SDL_Color colors[256];

		for(int j = 0 ; j < 256 ; j++)
		{
			colors[j].r = 0;
			colors[j].g = 0;
			colors[j].b = 0;
		}

		SDL_SetPalette(temp, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);
	}
	//else if((*bitmap_size) == (*width)*(*height)*3)
	//{
	//	SDL_Surface *temp = IMG_Load_RW(rw, 1);
	//}

	IMG_SavePNG(temp, name);

	SDL_FreeSurface(temp);

	return true;
}

int main( int argc, char * argv[] )
{
	std::ifstream infile;
	//infile.open( "H3bitmap.lod" );
	infile.open( "H3ab_bmp.lod" );

	Header header;

	if( infile.is_open() )
	{
		infile.read(header.r, sizeof(header.r));

		for( uint32_t i = 0 ; i < header.o.files_count ; i++ )
		{
			File f;
			infile.read(f.r, sizeof(f.r));

			std::string filePath = "files/";
			filePath += f.o.name;

			bool isCompressed = true;

			uint32_t inFileSize = f.o.size_compressed;
			uint32_t outFileSize = f.o.size_original;

			if(f.o.size_compressed == 0)
			{
				isCompressed = false;
				inFileSize = f.o.size_original;
			}

			char* buffer = new char[inFileSize];
			char* outBuffer = new char[outFileSize];

			int lastPos = infile.tellg();
			infile.seekg(f.o.offset, infile.beg);
			infile.read(buffer, inFileSize);

			if(isCompressed)
			{
				// zlib struct
				z_stream infstream;
				infstream.zalloc = Z_NULL;
				infstream.zfree = Z_NULL;
				infstream.opaque = Z_NULL;

				// setup "b" as the input and "c" as the compressed output
				infstream.avail_in = (uInt)inFileSize; // size of input
				infstream.next_in = (Bytef *)buffer; // input char array
				infstream.avail_out = (uInt)outFileSize; // size of output
				infstream.next_out = (Bytef *)outBuffer; // output char array

				// the actual DE-compression work.
				inflateInit(&infstream);
				inflate(&infstream, Z_NO_FLUSH);
				inflateEnd(&infstream);
			}
			else
			{
				memcpy(outBuffer, buffer, inFileSize);
			}

			if(isPCX(outBuffer, outFileSize))
			{
				convertToPNG(outBuffer, outFileSize, filePath.c_str());
			}
			else
			{
				std::ofstream outfile;

				outfile.open(filePath.c_str());

				outfile.write(outBuffer, outFileSize);

				outfile.close();
			}

			delete[] buffer;
			delete[] outBuffer;

			infile.seekg(lastPos, infile.beg);
		}
	}

	infile.close();

	return EXIT_SUCCESS;
	/*
	boost::program_options::options_description description( "options" );
	description.add_options()
	( "help", "print help message and instructions" )
	( "thread",
	boost::program_options::value< size_t >(),
	"number of threads to run" );
	try
	{
		boost::program_options::variables_map variableMap;
		boost::program_options::store(
		boost::program_options::parse_command_line( argc, argv, description ), variableMap );
		boost::program_options::notify( variableMap );
		size_t numberOfThreads = 0;
		if( variableMap.count( "help" ) != 0 )
		{
			std::cout << description << std::endl;
			return EXIT_FAILURE;
		}
		else if( variableMap.count( "thread" ) != 0 )
		{
			numberOfThreads = variableMap[ "thread" ].as< size_t >();
		}
		else
		{
			std::cout << description << std::endl;
			return EXIT_FAILURE;
		}
	}
	catch( boost::program_options::error& e )
	{
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		std::cerr << description << std::endl;
		return EXIT_FAILURE;
	}
	catch(std::exception& e)
	{
		std::cerr << "Unhandled Exception reached the top of main: "
		<< e.what() << ", application will now exit" << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
	*/
}

