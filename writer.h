#ifdef __WAND__
target[name[writer.h] type[include]]
dependency[writer.o]
#endif

#ifndef IMAGIO_WRITER_H
#define IMAGIO_WRITER_H

namespace Herbs
	{
	class StreamOut;
	};

namespace Vector
	{
	template<class T>
	class MatrixStorage;
	}

namespace ImagIO
	{
	struct Pixel;
	struct Metadata;

	class Writer
		{
		public:
			Writer(Herbs::StreamOut& source);
			~Writer();
			void dataWrite(const Vector::MatrixStorage<Pixel>& pixels
				,const Metadata& data);
			
		private:
			void cleanup();
			
			Herbs::StreamOut& m_dest;
			void* write_handle;
			void* info_handle;
		};
	}

#endif
