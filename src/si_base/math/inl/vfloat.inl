#pragma once

#include <smmintrin.h>

#include "si_base/math/vfloat.h"
#include "si_base/math/math_internal.h"

namespace SI
{
	inline Vfloat::Vfloat()
		: m_v(kSiFloat128_0000)
	{
	}
	
	inline Vfloat::Vfloat(const Vfloat& v)
		: m_v(v.m_v)
	{
	}
		
	inline Vfloat::Vfloat(float value)
		: m_v(_mm_set_ss(value))
	{
	}
		
	inline Vfloat::Vfloat(const __m128& v)
		: m_v(v)
	{
	}
	
	inline void Vfloat::Set(float v)
	{
		m_v = _mm_set_ss(v);
	}

	inline float Vfloat::Get() const
	{
		return _mm_cvtss_f32(m_v);
	}
		
	inline float Vfloat::AsFloat() const
	{
		return _mm_cvtss_f32(m_v);
	}

	inline Vfloat::operator float() const
	{
		return _mm_cvtss_f32(m_v);
	}

	inline Vfloat Vfloat::operator+(Vfloat_arg r) const
	{
		return Vfloat(_mm_add_ss(m_v, r.m_v));
	}

	inline Vfloat Vfloat::operator-(Vfloat_arg r) const
	{
		return Vfloat(_mm_sub_ss(m_v, r.m_v));
	}

	inline Vfloat Vfloat::operator*(Vfloat_arg r) const
	{
		return Vfloat(_mm_mul_ss(m_v, r.m_v));
	}

	inline Vfloat Vfloat::operator/(Vfloat_arg r) const
	{
		return Vfloat(_mm_div_ss(m_v, r.m_v));
	}

	inline __m128 Vfloat::Get128() const
	{
		return m_v;
	}

	///////////////////////////////////////////////////////////////////
	
	inline Vfloat& Vfloat::operator=(const Vfloat& v)
	{
		this->m_v = v.m_v;
		return (*this);
	}

	inline Vfloat operator+(float f, Vfloat_arg vf)
	{
		return Vfloat(f) + vf;
	}
	
	inline Vfloat operator-(float f, Vfloat_arg vf)
	{
		return Vfloat(f) - vf;
	}
	
	inline Vfloat operator*(float f, Vfloat_arg vf)
	{
		return Vfloat(f) * vf;
	}
	
	inline Vfloat operator/(float f, Vfloat_arg vf)
	{
		return Vfloat(f) / vf;
	}

	inline Vfloat operator+(Vfloat_arg vf, float f)
	{
		return vf + Vfloat(f);
	}
	
	inline Vfloat operator-(Vfloat_arg vf, float f)
	{
		return vf - Vfloat(f);
	}
	
	inline Vfloat operator*(Vfloat_arg vf, float f)
	{
		return vf * Vfloat(f);
	}
	
	inline Vfloat operator/(Vfloat_arg vf, float f)
	{
		return vf / Vfloat(f);
	}

	namespace Math
	{
		inline Vfloat Min(const Vfloat& a, const Vfloat& b)
		{
			return Vfloat(_mm_min_ss(a.Get128(), b.Get128()));
		}

		inline Vfloat Max(const Vfloat& a, const Vfloat& b)
		{
			return Vfloat(_mm_max_ss(a.Get128(), b.Get128()));
		}

		inline Vfloat Abs(const Vfloat& a)
		{
			return Vfloat(_mm_and_ps(a.Get128(), kSiUint128_AbsMask));
		}

		inline Vfloat Sqrt(const Vfloat& a)
		{
			return Vfloat(_mm_sqrt_ss(a.Get128()));
		}

		inline Vfloat Rsqrt(const Vfloat& a)
		{
			return Vfloat(_mm_rsqrt_ss(a.Get128()));
		}

		inline Vfloat Rcp(const Vfloat& a)
		{
			return Vfloat(_mm_rcp_ss(a.Get128()));
		}

		inline Vfloat Floor(const Vfloat& a)
		{
			__m128i int128 = _mm_cvtps_epi32(a.Get128());
			__m128 float128 = _mm_cvtepi32_ps(int128);
			__m128 floor128 = _mm_sub_ps(float128, _mm_and_ps(_mm_cmplt_ps(a.Get128(), float128), kSiFloat128_1111));

			return Vfloat(floor128);
		}
		
	} // namespace Math
}
