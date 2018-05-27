#pragma once

#include "si_base/math/vfloat.h"
#include "si_base/math/math_internal.h"

namespace SI
{
	inline Vfloat::Vfloat()
		: m_v(kSiFloat128_0000)
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

	inline Vfloat Vfloat::operator+(Vfloat_arg r)
	{
		return Vfloat(_mm_add_ss(m_v, r.m_v));
	}

	inline Vfloat Vfloat::operator-(Vfloat_arg r)
	{
		return Vfloat(_mm_sub_ss(m_v, r.m_v));
	}

	inline Vfloat Vfloat::operator*(Vfloat_arg r)
	{
		return Vfloat(_mm_mul_ss(m_v, r.m_v));
	}

	inline Vfloat Vfloat::operator/(Vfloat_arg r)
	{
		return Vfloat(_mm_div_ss(m_v, r.m_v));
	}

	inline __m128 Vfloat::Get128() const
	{
		return m_v;
	}

	///////////////////////////////////////////////////////////////////

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
		
	} // namespace Math
}
