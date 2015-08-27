#include <cstdlib>
#include <fstream>
#include <iostream>

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

int main( int argc, char * argv[] )
{
	std::ifstream infile;
	infile.open( "H3bitmap.lod" );

	H3lod h3lod;

	Header header;

	if( infile.is_open() )
	{
		infile.read(header.r, sizeof(header.r));
		//std::cout << header.o.magic << std::endl;
		//std::cout << header.o.type << std::endl;
		//std::cout << header.o.files_count << std::endl;
		
		
		//infile.read(buffer, sizeof h3lod.magic);
		//std::cout << buffer << std::endl;
		//infile.read(buffer1, sizeof h3lod.type);
		//std::cout << std::hex << buffer1[0] << buffer1[1] << buffer1[2] << buffer1[3] << std::endl;
		//infile.read(buffer2, sizeof h3lod.files_count);
		//std::cout <<std::hex <<  buffer2[0] << buffer2[1] << buffer2[2] << buffer2[3] << std::endl;
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

