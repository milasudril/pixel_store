#ifdef __WAND__
target[name[reader.h] type[include]]
dependency[reader.o]
#endif

#ifndef IMAGIO_READER_H
#define IMAGIO_READER_H

namespace Herbs
	{
	class StreamIn;
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

	class Reader
		{
		public:
			Reader(Herbs::StreamIn& source);
			~Reader();
			void dataRead(Vector::MatrixStorage<Pixel>& pixels
				,Metadata& data);
			
		private:
			void cleanup();
			
			Herbs::StreamIn& m_source;
			void* read_handle;
			void* info_handle;
			void* info_end_handle;
		};
	}

#endif
