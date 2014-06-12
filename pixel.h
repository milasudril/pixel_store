#ifdef __WAND__
target[name[pixel.h] type[include]]
#endif

#ifndef IMAGIO_PIXEL_H
#define IMAGIO_PIXEL_H

namespace ImagIO
	{
	struct Pixel
		{
		float red;
		float green;
		float blue;
		float alpha;
		};
		
//TODO	Add pixel operations like
	inline Pixel blend(Pixel fg, Pixel bg)
		{
		return 
			{
			fg.red*fg.alpha + bg.red*(1-fg.alpha)
			,fg.green*fg.alpha + bg.green*(1-fg.alpha)
			,fg.blue*fg.alpha + bg.blue*(1-fg.alpha)
			,fg.alpha*fg.alpha + bg.alpha*(1-fg.alpha)
			};
		}
	}

#endif
