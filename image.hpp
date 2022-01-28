#ifndef PIXELSTORE_IMAGE_HPP
#define PIXELSTORE_IMAGE_HPP

#include <memory>

namespace pixel_store
{
	template<class T>
	class image
	{
	public:
		explicit image(uint32_t width, uint32_t height):
			m_pixels{std::make_unique<T[]>(static_cast<size_t>(width) *  static_Cast<size_t>(height))},
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
		std::unique_ptr<T[]> m_pixels;
		size_t m_width;
		size_t m_height;
	};
}

#endif
