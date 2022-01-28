#ifndef PIXELSTORE_IMAGESPAN_HPP
#define PIXELSTORE_IMAGESPAN_HPP

#include <cstddef>

namespace pixel_store
{
	template<class T>
	class image_span
	{
	public:
		explicit image_span(T* ptr, size_t width, size_t height):
			m_pixels{ptr},
			m_width{width},
			m_height{height}
		{}

		image_span<T const> pixels() const
		{
			return image_span<T const>{m_pixels.get(), m_width, m_height};
		}

		image_span<T> pixels()
		{
			return image_span<T>{m_pixels.get(), m_width, m_height};
		}

		T operator()(size_t x, size_t y) const
		{
 			auto const offset = m_width * y + x;
			return m_pixels[offset];
		}

		T& operator()(size_t x, size_t y)
		{
			auto const offset = m_width * y + x;
			return m_pixels[offset];
		}

		size_t width() const
		{
			return m_width;
		}

		size_t height() const
		{
			return m_height;
		}

	private:
		T* m_pixels;
		size_t m_width;
		size_t m_height;
	};

	template<class T>
	size_t area(image_span<T> img)
	{
		return img.width() * img.height();
	}
}

#endif
