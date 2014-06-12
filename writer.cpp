#ifdef __WAND__
target[name[writer.o] type[object] dependency[png;external]]
#endif

#include "writer.h"
#include "pixel.h"
#include "pixelsconvert.h"
#include "metadata.h"
#include <vector/matrixstorage.h>
#include <herbs/streamout/streamout.h>
#include <herbs/exceptionmissing/exceptionmissing.h>
#include <herbs/string/string.h>
#include <herbs/stringformat/stringformat.h>
#include <herbs/logmessage/logmessage.h>
#include <herbs/logwriter/logwriter.h>
#include <herbs/cpuinfo/cpuinfo.h>

#define PNG_USER_MEM_SUPPORTED
#include <png.h>

namespace
	{
	class PngException:public Herbs::Exception
		{
		public:
			PngException(png_const_charp errmesg)
				{
				while(*errmesg!='\0')
					{
					message.append(*errmesg);
					++errmesg;
					}
				}
			
			void print(Herbs::LogWriter& writer) const
				{
				auto message_out=Herbs::format(
					STR("Det gick inte att skriva PNG-filen. %0")
					,{
					message.begin()
					});
				writer.write({message_out.begin(),Herbs::LogMessage::Type::ERROR});
				}
		private:
			Herbs::String message;
		};
		
	static const unsigned int N_bytes_check=8;
	
	void pngError(png_structp png_ptr,png_const_charp errmesg)
		{
		throw PngException(errmesg);
		}
	
	png_voidp pngMalloc(png_structp png_ptr,png_size_t size)
		{
		return Herbs::malloc_pod(size);
		}

	void pngFree(png_structp png_ptr, png_voidp ptr)
		{
		return Herbs::free_pod(ptr);
		}
	
	void pngWrite(png_structp png_ptr, png_bytep data, png_size_t length)
		{
		Herbs::StreamOut* dest=(Herbs::StreamOut*)png_get_io_ptr(png_ptr);
		if(dest->write(data,length)!=length)
			{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
		}
	
	void pngFlush(png_structp png_ptr)
		{
		Herbs::StreamOut* dest=(Herbs::StreamOut*)png_get_io_ptr(png_ptr);
		dest->flush();
		}
	
	};

ImagIO::Writer::Writer(Herbs::StreamOut& dest):m_dest(dest)
	{

	write_handle=nullptr;
	info_handle=nullptr;
	
	write_handle=png_create_write_struct_2(PNG_LIBPNG_VER_STRING
		,&m_dest,pngError,pngError
		,nullptr,pngMalloc,pngFree);
	if(write_handle==NULL)
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
		
	info_handle=png_create_info_struct((png_structp)write_handle);
	if(info_handle==NULL)
		{
		cleanup();
		throw Herbs::ExceptionMissing(___FILE__,__LINE__);
		}

	png_set_write_fn((png_structp)write_handle,&m_dest,pngWrite,pngFlush);
	}
	
void ImagIO::Writer::cleanup()
	{
	png_structpp w=(write_handle==nullptr?nullptr:(png_structpp)(&write_handle));
	png_infopp i=(info_handle==nullptr?nullptr:(png_infopp)(&info_handle));
	png_destroy_write_struct(w,i);
	}

ImagIO::Writer::~Writer()
	{
	cleanup();
	}

void ImagIO::Writer::dataWrite(const Vector::MatrixStorage<Pixel>& pixels
	,const Metadata& data)
	{
	png_set_IHDR((png_structp)write_handle,(png_infop)info_handle
		,pixels.nColsGet(),pixels.nRowsGet(),16,PNG_COLOR_TYPE_RGB_ALPHA
		,PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT
		,PNG_FILTER_TYPE_DEFAULT);
	png_set_pHYs((png_structp)write_handle,(png_infop)info_handle
		,data.resolution_x, data.resolution_y,PNG_RESOLUTION_METER);
		
	Vector::MatrixStorage<uint16_t> temp(pixels.nRowsGet(),pixels.nColsGet()*4);
	convert((float*)pixels.rowGet(0),temp.rowGet(0)
		,pixels.nRowsGet()*pixels.nColsGet()*4);
		
	uint32_t data_transforms=PNG_TRANSFORM_IDENTITY;
	if(!Herbs::CPUInfo::bigEndianIs())
		{data_transforms|=PNG_TRANSFORM_SWAP_ENDIAN;}
	
	png_set_rows((png_structp)write_handle,(png_infop)info_handle
		,(png_bytepp)temp.rowsGet());
	png_write_png((png_structp)write_handle,(png_infop)info_handle
		,data_transforms, NULL);
	}
