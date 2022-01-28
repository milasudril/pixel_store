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
		T red() const
		{ return m_value[0]; }

		T green() const
		{ return m_value[1]; }

		T blue() const
		{ return m_value[2]; }

		T alpha() const
		{ return m_value[2]; }

		T& red()
		{ return m_value[0]; }

		T& green()
		{ return m_value[1]; }

		T& blue()
		{ return m_value[2]; }

		T& alpha()
		{ return m_value[2]; }

		vec4_t<T> value() const
		{ return m_value; }

		vec4_t<T>& value()
		{ return m_value; }

	private:
		vec4_t<T> m_value;
	};
}

#endif
