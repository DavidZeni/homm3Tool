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

