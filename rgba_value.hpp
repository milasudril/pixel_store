#ifndef PIXELSTORE_RGBAVALUE_HPP
#define PIXELSTORE_RGBAVALUE_HPP

namespace pixel_store
{
	template<class T>
	using vec4_t [[gnu::vector_size(4*sizeof(T))]] = T;

	template<class T>
	class rgba_value
	{
	public:
        rgba_value() = default;

		constexpr explicit rgba_value(T r, T g, T b, T a):m_value{r, g, b, a}{}

		constexpr T red() const
		{ return m_value[0]; }

		constexpr T& red()
		{ return m_value[0]; }

		constexpr T green() const
		{ return m_value[1]; }

		constexpr T& green()
		{ return m_value[1]; }

		constexpr T blue() const
		{ return m_value[2]; }

		constexpr T& blue()
		{ return m_value[2]; }

		constexpr T alpha() const
		{ return m_value[3]; }

		constexpr T& alpha()
		{ return m_value[3]; }

		constexpr vec4_t<T> value() const
		{ return m_value; }

		constexpr vec4_t<T>& value()
		{ return m_value; }

	private:
		vec4_t<T> m_value;
	};

	template<class T>
	constexpr auto red_v = rgba_value<T>{1, 0, 0, 1};

	template<class T>
	constexpr auto green_v = rgba_value<T>{0, 1, 0, 1};

	template<class T>
	constexpr auto blue_v = rgba_value<T>{0, 0, 1, 1};

	template<class T>
	constexpr auto white_v rgba_value<T>{1, 1, 1, 1};

	template<class T>
	constexpr auto cyan_v = rgba_value<T>{0, 1, 1, 1};

	template<class T>
	constexpr auto magenta_v = rgba_value<T>{1, 0, 1, 1};

	template<class T>
	constexpr auto yellow_v = rgba_value<T>{1, 1, 0, 1};

	template<class T>
	constexpr auto black_v rgba_value<T>{0, 0, 0, 1};

	template<class T>
	constexpr auto operator==(rgba_value<T> a, rgba_value<T> b)
	{
		auto const val = a.value() - b.value();
		return val[0] == 0 && val[1]==0 && val[2]==0 && val[3]==0;
	}

	template<class T>
	constexpr auto operator!=(rgba_value<T> a, rgba_value<T> b)
	{
		return !(a == b);
	}
}

#endif
