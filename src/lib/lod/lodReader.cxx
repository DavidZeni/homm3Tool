#include "lib/lod/lodReader.hxx"

namespace lodReader
{

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

}