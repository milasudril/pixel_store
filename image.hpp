#ifndef PIXELSTORE_IMAGE_HPP
#define PIXELSTORE_IMAGE_HPP

#include "./image_span.hpp"

#include <memory>
#include <algorithm>

namespace pixel_store
{
	template<class T>
	class image
	{
	public:
		image():m_pixels{nullptr}, m_width{0}, m_height{0} {}

		explicit image(uint32_t width, uint32_t height):
			m_pixels{std::make_unique<T[]>(static_cast<size_t>(width) * static_cast<size_t>(height))},
			m_width{width},
			m_height{height}
		{}

		image(image&&) = default;
		image& operator=(image&&) = default;

		explicit image(image_span<T const> img):image{img.width(), img.height()}
		{
			std::copy_n(img.m_pixels.get(), width()*height(), m_pixels.get());
		}

		image(image const& other):image{other.pixels()}{}

		image& operator=(image const& other)
		{
			image tmp{other};
			*this = std::move(other);
			return *this;
		}

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

		T* data()
		{
			return m_pixels.get();
		}

		T const* data() const
		{
			return m_pixels.get();
		}

	private:
		std::unique_ptr<T[]> m_pixels;
		size_t m_width;
		size_t m_height;
	};

	template<class T>
	size_t area(image<T> const& img)
	{
		return img.width() * img.height();
	}

	template<class T>
	auto make_span(image<T> const& img)
	{
		return std::span{std::data(img), area(img)};
	}
}

#endif
