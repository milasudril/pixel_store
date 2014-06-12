#ifdef __WAND__
target[name[reader.o] type[object] dependency[png;external]]
#endif

#include "reader.h"
#include "pixel.h"
#include "pixelsconvert.h"
#include "metadata.h"
#include <vector/matrixstorage.h>
#include <herbs/streamin/streamin.h>
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
					STR("Det gick inte att avkoda PNG-filen. %0")
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
	
	void pngRead(png_structp png_ptr, png_bytep data, png_size_t length)
		{
		Herbs::StreamIn* source=(Herbs::StreamIn*)png_get_io_ptr(png_ptr);
		if(source->read(data,length)!=length)
			{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
		}
	
	};

ImagIO::Reader::Reader(Herbs::StreamIn& source):m_source(source)
	{
	uint8_t buffer[N_bytes_check];
	
		{
		m_source.modeBufferedOn();
		auto offset_init=m_source.offsetGet();
		uint8_t* p_buffer=buffer;
		uint8_t n=N_bytes_check;
		while(!m_source.eof() && n!=0)
			{
			*p_buffer=m_source.byteGet();
			++p_buffer;
			--n;
			}
		m_source.offsetRestore(offset_init);
		m_source.modeBufferedOff();
		}

	if(png_sig_cmp(buffer,0,N_bytes_check)!=0)
		{
		throw Herbs::ExceptionMissing(___FILE__,__LINE__);
		}
	read_handle=nullptr;
	info_handle=nullptr;
	info_end_handle=nullptr;
	
	read_handle=png_create_read_struct_2(PNG_LIBPNG_VER_STRING
		,&m_source,pngError,pngError
		,nullptr,pngMalloc,pngFree);
	if(read_handle==NULL)
		{throw Herbs::ExceptionMissing(___FILE__,__LINE__);}
		
	info_handle=png_create_info_struct((png_structp)read_handle);
	info_end_handle=png_create_info_struct((png_structp)read_handle);
	if(info_handle==NULL || info_end_handle==NULL)
		{
		cleanup();
		throw Herbs::ExceptionMissing(___FILE__,__LINE__);
		}
	
	png_set_sig_bytes((png_structp)read_handle,0);
	png_set_read_fn((png_structp)read_handle,&m_source,pngRead);
	}
	
void ImagIO::Reader::cleanup()
	{
	png_structpp r=(read_handle==nullptr?nullptr:(png_structpp)(&read_handle));
	png_infopp i=(info_handle==nullptr?nullptr:(png_infopp)(&info_handle));
	png_infopp e=(info_end_handle==nullptr?nullptr:(png_infopp)(&info_end_handle));
	png_destroy_read_struct(r,i,e);
	}

ImagIO::Reader::~Reader()
	{
	cleanup();
	}

void ImagIO::Reader::dataRead(Vector::MatrixStorage<Pixel>& pixels
	,Metadata& data)
	{
	png_read_info((png_structp)read_handle,(png_infop)info_handle);
	
	auto type_color=png_get_color_type((png_structp)read_handle,(png_infop)info_handle);
	auto n_bits=png_get_bit_depth((png_structp)read_handle,(png_infop)info_handle);
	uint32_t filler=0xff;
	switch(n_bits)
		{
		case 16:
			filler=0xffff;
			break;
		case 32: //This value is not defined by the standard, though it should be
			filler=0xffffffffu;
			break;
		}
	
	switch(type_color)
		{
		case PNG_COLOR_TYPE_RGB:
			png_set_add_alpha((png_structp)read_handle,filler,PNG_FILLER_AFTER);
			break;
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb((png_structp)read_handle);
			png_set_add_alpha((png_structp)read_handle,filler,PNG_FILLER_AFTER);
			break;
		case PNG_COLOR_TYPE_GRAY:
			if(n_bits < 8)
				{png_set_expand_gray_1_2_4_to_8((png_structp)read_handle);}
			png_set_add_alpha((png_structp)read_handle,filler,PNG_FILLER_AFTER);
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			png_set_gray_to_rgb((png_structp)read_handle);
			break;
		}

	if(png_get_valid((png_structp)read_handle,(png_infop)info_handle
		,PNG_INFO_tRNS))
		{png_set_tRNS_to_alpha((png_structp)read_handle);}
	
	if(n_bits < 8)
		{png_set_packing((png_structp)read_handle);}
		
	if(!Herbs::CPUInfo::bigEndianIs() && n_bits>8)
		{png_set_swap((png_structp)read_handle);}

		{
		int unit;
		uint32_t res_x;
		uint32_t res_y;
		png_get_pHYs((png_structp)read_handle,(png_infop)info_handle
			, &res_x, &res_y,&unit);
		data.resolution_x=res_x;
		data.resolution_y=res_y;
		}
		
	png_read_update_info((png_structp)read_handle,(png_infop)info_handle);
	
	n_bits=png_get_bit_depth((png_structp)read_handle,(png_infop)info_handle);
	auto n_bytes_row=png_get_rowbytes((png_structp)read_handle,(png_infop)info_handle);
	
	size_t width=png_get_image_width((png_structp)read_handle,(png_infop)info_handle);
	size_t height=png_get_image_height((png_structp)read_handle,(png_infop)info_handle);
	
	Vector::MatrixStorage<uint8_t> pixels_temp(height,n_bytes_row);
	png_read_image((png_structp)read_handle, (png_bytepp)pixels_temp.rowsGet());
	png_read_end((png_structp)read_handle,(png_infop)info_end_handle);
	
	pixels.resize(height,width);
	switch(n_bits)
		{
		case 8:
			convert(pixels_temp.rowGet(0),(float*)(pixels.rowGet(0))
				,4*width*height);
			break;
		case 16:
			convert((uint16_t*)pixels_temp.rowGet(0)
				,(float*)(pixels.rowGet(0))
				,4*width*height);
		}
	}