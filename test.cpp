#ifdef __WAND__
target[name[imagio_test.exe] type[application] platform[;Windows]]
target[name[imagio_test] type[application]]
#endif

#include "reader.h"
#include "writer.h"
#include "pixel.h"
#include "metadata.h"
#include <vector/matrixstorage.h>
#include <herbs/filein/filein.h>
#include <herbs/fileout/fileout.h>
#include <herbs/exception/exception.h>
#include <herbs/logwriterdefault/logwriterdefault.h>
#include <herbs/main/main.h>

int MAIN(int argc,charsys_t* argv[])
	{
	Herbs::LogWriterDefault log;

	try
		{
		Herbs::FileIn source(Herbs::Path(STR("test.png")));
		ImagIO::Reader reader(source);
		
		Vector::MatrixStorage<ImagIO::Pixel> pixels(1,1);
		ImagIO::Metadata metadata;
		
		reader.dataRead(pixels,metadata);
		printf("%llu x %llu\n",pixels.nColsGet(),pixels.nRowsGet());
		
		
		Herbs::FileOut dest(Herbs::Path(STR("test2.png")));
		ImagIO::Writer writer(dest);
		writer.dataWrite(pixels,metadata);
		
		}
	catch(const Herbs::Exception& e)
		{
		e.print(log);
		}
	
	return 0;
	}