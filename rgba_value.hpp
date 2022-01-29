#ifndef PIXELSTORE_RGBAVALUE_HPP
#define PIXELSTORE_RGBAVALUE_HPP

#include <cmath>
#include <cstdint>

namespace pixel_store
{
	template<class T>
	using vec4_t [[gnu::vector_size(4*sizeof(T))]] = T;

	template<class To, class From>
	constexpr auto vector_cast(From from)
	{
		return vec4_t<To>{static_cast<To>(from[0]),
			static_cast<To>(from[1]),
			static_cast<To>(from[2]),
			static_cast<To>(from[3]),
		};
	}

	namespace color_profiles
	{
		struct linear
		{
			template<class T>
			static constexpr auto to_linear(vec4_t<T> val) {return val;}

			template<class T>
			static constexpr auto from_linear(vec4_t<T> val) {return val;}
		};

		template<int Num = 22, int Denom = 10>
		struct gamma
		{
			static constexpr auto to_linear(vec4_t<float> val)
			{
				constexpr auto gamma_val = static_cast<float>(Num)/static_cast<float>(Denom);
				return vec4_t<float>{std::pow(val[0], 1.0f/gamma_val),
					std::pow(val[1], 1.0f/gamma_val),
					std::pow(val[2], 1.0f/gamma_val),
					std::pow(val[3], 1.0f/gamma_val)};
			}

			static constexpr auto from_linear(vec4_t<float> val)
			{
				constexpr auto gamma_val = static_cast<float>(Num)/static_cast<float>(Denom);
				return vec4_t<float>{std::pow(val[0], gamma_val),
					std::pow(val[1], gamma_val),
					std::pow(val[2], gamma_val),
					std::pow(val[3], gamma_val)};
			}
		};
	}

	template<class T = float, class ColorProfile = color_profiles::linear>
	class rgba_value
	{
	public:
		using value_type = T;
		using color_profile = ColorProfile;

        rgba_value() = default;

		constexpr explicit rgba_value(T r, T g, T b, T a):m_value{r, g, b, a}{}

		constexpr explicit rgba_value(vec4_t<T> val): m_value{val}{}

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

	template<class To>
	constexpr To convert_to(rgba_value<float> from);

	template<class To>
	constexpr To convert_to(rgba_value<uint8_t, color_profiles::gamma<>> from);

	template<>
	constexpr rgba_value<uint8_t, color_profiles::gamma<>>
	convert_to<rgba_value<uint8_t, color_profiles::gamma<>>>(rgba_value<float> from)
	{
		using profile = color_profiles::gamma<>;
		auto val = 255.0f*profile::from_linear(from.value());
		return rgba_value<uint8_t, profile>{vector_cast<uint8_t>(val)};
	}

	template<>
	constexpr rgba_value<> convert_to<rgba_value<>>(rgba_value<uint8_t, color_profiles::gamma<>> from)
	{
		auto val = vector_cast<float>(from.value())/255.0f;
		using profile = color_profiles::gamma<>;
		return rgba_value<>{profile::to_linear(val)};
	}

	namespace standard_colors
	{
		template<class T>
		constexpr auto red_v = rgba_value<T>{1, 0, 0, 1};

		template<class T>
		constexpr auto green_v = rgba_value<T>{0, 1, 0, 1};

		template<class T>
		constexpr auto blue_v = rgba_value<T>{0, 0, 1, 1};

		template<class T>
		constexpr auto white_v = rgba_value<T>{1, 1, 1, 1};

		template<class T>
		constexpr auto cyan_v = rgba_value<T>{0, 1, 1, 1};

		template<class T>
		constexpr auto magenta_v = rgba_value<T>{1, 0, 1, 1};

		template<class T>
		constexpr auto yellow_v = rgba_value<T>{1, 1, 0, 1};

		template<class T>
		constexpr auto black_v = rgba_value<T>{0, 0, 0, 1};
	}
}

#endif
