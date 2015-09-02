#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "zlib.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_video.h"

//#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

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

bool saveToPNG( char* buffer, uint32_t bufferSize, std::string& filePath )
{
	int it = 0;

	uint32_t bitmap_size = *((uint32_t*)(buffer + it));
	it += 4;
	uint32_t width = *((uint32_t*)(buffer + it));
	it += 4;
	uint32_t height = *((uint32_t*)(buffer + it));
	it += 4;

	SDL_Surface *surface = nullptr;

	if(bitmap_size == width * height)
	{
		surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 8, 0, 0, 0, 0);

		it = 0xC;
		for (int i = 0; i < height; i++)
		{
			memcpy((char*)surface->pixels + surface->pitch * i, buffer + it, width);
			it+= width;
		}

		//palette - last 256*3 bytes
		it = bufferSize-256*3;
		for (int i=0;i<256;i++)
		{
			SDL_Color tp;
			tp.r = buffer[it++];
			tp.g = buffer[it++];
			tp.b = buffer[it++];
			tp.a = SDL_ALPHA_OPAQUE;
			surface->format->palette->colors[i] = tp;
		}
	}
	else if(bitmap_size == width * height * 3)
	{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		int bmask = 0xff0000;
		int gmask = 0x00ff00;
		int rmask = 0x0000ff;
#else
		int bmask = 0x0000ff;
		int gmask = 0x00ff00;
		int rmask = 0xff0000;
#endif
		surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, rmask, gmask, bmask, 0);

		for (int i = 0; i < height; i++)
		{
			memcpy((char*)surface->pixels + surface->pitch * i, buffer + it, width * 3);
			it += width * 3;
		}

	}
	else
	{
		SDL_FreeSurface(surface);
		return false;
	}

	std::size_t found = filePath.find_last_of(".");

	filePath.replace(filePath.begin()+found, filePath.end(), ".png");

	IMG_SavePNG(surface, filePath.c_str());

	SDL_FreeSurface(surface);

	return true;
}

int main( int argc, char * argv[] )
{
	std::ifstream infile;
	//infile.open( "H3bitmap.lod" );
	infile.open( "H3bitmap.lod" );

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
				saveToPNG(outBuffer, outFileSize, filePath);
			}
			else
			{
				std::ofstream outfile;

				outfile.open(filePath);

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

