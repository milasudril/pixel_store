#ifdef __WAND__
target[name[pixelsconvert.h] type[include]]
#endif

#ifndef IMAGIO_PIXELSCONVERT_H
#define IMAGIO_PIXELSCONVERT_H

#include <cstdint>
#include <cstddef>

namespace ImagIO
	{
	template<class To, class From>
	To convert( From src );

	template<>
	inline float convert<float,uint8_t>(uint8_t val_in)
		{return val_in/255.0f;}

	template<>
	inline float convert<float,uint16_t>(uint16_t val_in)
		{return val_in/65535.0f;}

	template<>
	inline uint8_t convert<uint8_t,float>(float val_in)
		{return 255*val_in;}
	template<>
	inline uint16_t convert<uint16_t,float>(float val_in)
		{return 65535*val_in;}

	template<class From,class To>
	void convert(const From* from, To* to, size_t N)
		{
		while(N)
			{
			*to=convert<To>(*from);
			++to;
			++from;
			--N;
			}
		}
	}

#endif
