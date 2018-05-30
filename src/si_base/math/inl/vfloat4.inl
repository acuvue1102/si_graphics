#pragma once

#include "si_base/math/vfloat4.h"

#include <xmmintrin.h>
#include <smmintrin.h>
#include "si_base/math/vfloat.h"
#include "si_base/math/vfloat3.h"
#include "si_base/math/vfloat4x4.h"
#include "si_base/core/assert.h"

namespace SI
{
	inline Vfloat4::Vfloat4()
		: m_v(kSiFloat128_0000)
	{
	}

	inline Vfloat4::Vfloat4(float x, float y, float z, float w)
		: m_v(_si_mm_set(x, y, z, w))
	{
	}
		
	inline Vfloat4::Vfloat4(Vfloat x, Vfloat y, Vfloat z, Vfloat w)
		: m_v(
			_mm_shuffle_ps(
				_mm_shuffle_ps(x.Get128(), y.Get128(), 0),
				_mm_shuffle_ps(z.Get128(), w.Get128(), 0),
				_MM_SHUFFLE(2,0,2,0))
		)
	{
	}
	
	inline Vfloat4::Vfloat4(Vfloat3 xyz, Vfloat w)
		: m_v(
			_mm_blend_ps(
				xyz.Get128(),
				_mm_shuffle_ps(w.Get128(), w.Get128(), 0),
				0b1000))
	{
	}
	
	inline Vfloat4::Vfloat4(Vfloat3 xyz, float w)
		: m_v(
			_mm_blend_ps(
				xyz.Get128(),
				_mm_set1_ps(w),
				0b1000))
	{
	}
		
	inline Vfloat4::Vfloat4(float value)
		: m_v(_mm_set1_ps(value))
	{
	}
		
	inline Vfloat4::Vfloat4(Vfloat_arg value)
		: m_v(_mm_shuffle_ps(value.Get128(), value.Get128(), 0))
	{
	}
		
	inline Vfloat4::Vfloat4(const float* v)
		: m_v(_mm_load_ps(v))
	{
	}

	inline Vfloat4::Vfloat4(__m128 v)
		: m_v(v)
	{
	}
		
	inline void Vfloat4::SetX(Vfloat_arg x)
	{
		m_v = _mm_blend_ps(m_v, x.Get128(), 0b0001);
	}

	inline void Vfloat4::SetY(Vfloat_arg y)
	{
		__m128 _y = _mm_shuffle_ps(y.Get128(), y.Get128(), 0);
		m_v = _mm_blend_ps(m_v, _y, 0b0010);
	}
		
	inline void Vfloat4::SetZ(Vfloat_arg z)
	{
		__m128 _z = _mm_shuffle_ps(z.Get128(), z.Get128(), 0);
		m_v = _mm_blend_ps(m_v, _z, 0b0100);
	}

	inline void Vfloat4::SetW(Vfloat_arg w)
	{
		__m128 _w = _mm_shuffle_ps(w.Get128(), w.Get128(), 0);
		m_v = _mm_blend_ps(m_v, _w, 0b1000);
	}

	inline void Vfloat4::SetElement(uint32_t elementIndex, Vfloat_arg e)
	{
		__m128 _e = _mm_shuffle_ps(e.Get128(), e.Get128(), 0);
		m_v = _si_mm_blend(m_v, _e, 1<<elementIndex);
	}

	inline void Vfloat4::Set(float value)
	{
		m_v = _mm_set1_ps(value);
	}

	inline void Vfloat4::Set(Vfloat_arg value)
	{
		m_v = _mm_shuffle_ps(value.Get128(), value.Get128(), 0);
	}
		
	inline void Vfloat4::Set(const float* v)
	{
		m_v = _mm_load_ps(v);
	}

	inline void Vfloat4::Set(float x, float y, float z, float w)
	{
		m_v = _si_mm_set(x, y, z, w);
	}

	inline void Vfloat4::Set(Vfloat_arg x, Vfloat_arg y, Vfloat_arg z, Vfloat_arg w)
	{
		__m128 xxyy = _mm_shuffle_ps(x.Get128(), y.Get128(), _MM_SHUFFLE(0,0,0,0));
		__m128 zzww = _mm_shuffle_ps(z.Get128(), w.Get128(), _MM_SHUFFLE(0,0,0,0));

		m_v = _mm_shuffle_ps(xxyy, zzww, _MM_SHUFFLE(2,0,2,0));
	}
	
	inline Vfloat Vfloat4::X() const
	{
		return Vfloat(m_v);
	}

	inline Vfloat Vfloat4::Y() const
	{
		return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(1,1,1,1)) );
	}

	inline Vfloat Vfloat4::Z() const
	{
		return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(2,2,2,2)) );
	}

	inline Vfloat Vfloat4::W() const
	{
		return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(3,3,3,3)) );
	}
	
	inline float Vfloat4::Xf() const
	{
		return X().AsFloat();
	}

	inline float Vfloat4::Yf() const
	{
		return Y().AsFloat();
	}

	inline float Vfloat4::Zf() const
	{
		return Z().AsFloat();
	}

	inline float Vfloat4::Wf() const
	{
		return W().AsFloat();
	}
		
	inline Vfloat Vfloat4::GetElement(uint32_t elementIndex) const
	{
		SI_ASSERT(elementIndex<4);
		switch(elementIndex)
		{
		case 0:
			return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(0,0,0,0)) );
		case 1:
			return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(1,1,1,1)) );
		case 2:
			return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(2,2,2,2)) );
		case 3:
			return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(3,3,3,3)) );
		}

		return Vfloat(kSiFloat128_0000);
	}

	template<uint32_t xIndex, uint32_t yIndex, uint32_t zIndex, uint32_t wIndex>
	inline Vfloat4 Vfloat4::Swizzle() const
	{
		return Vfloat4( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(wIndex, zIndex, yIndex, xIndex)) );
	}

	inline Vfloat4 Vfloat4::Swizzle(uint32_t xIndex, uint32_t yIndex, uint32_t zIndex, uint32_t wIndex) const
	{
		__m128 xyzwArray[4] = 
		{
			m_v,
			_mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(1,1,1,1)),
			_mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(2,2,2,2)),
			_mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(3,3,3,3))
		};

		return Vfloat4(
			_mm_shuffle_ps(
				_mm_shuffle_ps(xyzwArray[xIndex], xyzwArray[yIndex], 0),
				_mm_shuffle_ps(xyzwArray[zIndex], xyzwArray[wIndex], 0),
				_MM_SHUFFLE(2,0,2,0)));
	}

	inline Vfloat3 Vfloat4::XYZ() const
	{
		return Vfloat3(m_v);
	}
	
	inline Vfloat4 Vfloat4::XYZ0() const
	{
		Vfloat4(_mm_blend_ps(m_v, kSiFloat128_0000, 0b1000));
	}

	inline Vfloat4 Vfloat4::XYZ1() const
	{
		Vfloat4(_mm_blend_ps(m_v, kSiFloat128_1111, 0b1000));
	}
	
	inline Vfloat4 Vfloat4::Multiply(Vfloat4x4_arg m) const
	{
		return Math::Multiply(*this, m);
	}
	
	inline Vfloat Vfloat4::LengthSqr() const
	{
		return Math::LengthSqr(*this);
	}

	inline Vfloat Vfloat4::Length() const
	{
		return Math::Length(*this);
	}
	
	inline Vfloat4 Vfloat4::Normalize() const
	{
		return Math::Normalize(*this);
	}
	
	inline Vfloat4 Vfloat4::NormalizeFast() const
	{
		return Math::NormalizeFast(*this);
	}
	
	inline Vfloat4 Vfloat4::operator-() const
	{
		return Vfloat4( _mm_sub_ps( kSiFloat128_0000, Get128() ) );
	}

	inline Vfloat Vfloat4::operator[](size_t i) const && // [] operatorは代入を許可しないようにしておく.
	{
		return GetElement((uint32_t)i);
	}

	inline Vfloat4 Vfloat4::operator+(Vfloat4_arg v) const
	{
		return Vfloat4(_mm_add_ps(m_v, v.m_v));
	}

	inline Vfloat4 Vfloat4::operator-(Vfloat4_arg v) const
	{
		return Vfloat4(_mm_sub_ps(m_v, v.m_v));
	}

	inline Vfloat4 Vfloat4::operator*(Vfloat4_arg v) const
	{
		return Vfloat4(_mm_mul_ps(m_v, v.m_v));
	}

	inline Vfloat4 Vfloat4::operator/(Vfloat4_arg v) const
	{
		return Vfloat4(_mm_div_ps(m_v, v.m_v));
	}

	inline Vfloat4 Vfloat4::operator*(Vfloat_arg f) const
	{
		return (*this) * Vfloat4(f);
	}

	inline Vfloat4 Vfloat4::operator/(Vfloat_arg f) const
	{
		return (*this) / Vfloat4(f);
	}
	
	inline Vfloat4 Vfloat4::operator*(float f) const
	{
		return (*this) * Vfloat4(f);
	}

	inline Vfloat4 Vfloat4::operator/(float f) const
	{
		return (*this) / Vfloat4(f);
	}
	
	inline Vfloat4 Vfloat4::operator*(Vfloat4x4_arg m) const
	{
		return Math::Multiply(*this, m);
	}
	
	inline Vfloat4 Vfloat4::operator*(Vfloat4x3_arg m) const
	{
		return Math::Multiply(*this, m);
	}

	inline Vfloat4& Vfloat4::operator+=(Vfloat4_arg v)
	{
		*this = *this + v;
		return (*this);
	}

	inline Vfloat4& Vfloat4::operator-=(Vfloat4_arg v)
	{
		*this = *this - v;
		return (*this);
	}
		
	inline Vfloat4 Vfloat4::Zero()
	{
		return Vfloat4(kSiFloat128_0000.m_v);
	}

	inline Vfloat4 Vfloat4::One()
	{
		return Vfloat4(kSiFloat128_1111.m_v);
	}

	inline Vfloat4 Vfloat4::AxisX()
	{
		return Vfloat4(kSiFloat128_1000.m_v);
	}

	inline Vfloat4 Vfloat4::AxisY()
	{
		return Vfloat4(kSiFloat128_0100.m_v);
	}

	inline Vfloat4 Vfloat4::AxisZ()
	{
		return Vfloat4(kSiFloat128_0010.m_v);
	}

	inline Vfloat4 Vfloat4::AxisW()
	{
		return Vfloat4(kSiFloat128_0001.m_v);
	}

	inline __m128 Vfloat4::Get128() const
	{
		return m_v;
	}

	//////////////////////////////////////////////////////////////////////////
		
	inline Vfloat4 operator*(Vfloat f, Vfloat4_arg v )
	{
		return Vfloat4(f) * v;
	}
		
	inline Vfloat4 operator/(Vfloat f, Vfloat4_arg v )
	{
		return Vfloat4(f) / v;
	}

	inline Vfloat4 operator*(float f, Vfloat4_arg v)
	{
		return Vfloat4(f) * v;
	}

	inline Vfloat4 operator/(float f, Vfloat4_arg v)
	{
		return Vfloat4(f) / v;
	}

	namespace Math
	{
		inline Vfloat4 Min(Vfloat4_arg a, Vfloat4_arg b)
		{
			return Vfloat4(_mm_min_ps(a.Get128(), b.Get128()));
		}

		inline Vfloat4 Max(Vfloat4_arg a, Vfloat4_arg b)
		{
			return Vfloat4(_mm_max_ps(a.Get128(), b.Get128()));
		}

		inline Vfloat4 Sqrt(Vfloat4_arg a)
		{
			return Vfloat4(_mm_sqrt_ps(a.Get128()));
		}

		inline Vfloat4 Rsqrt(Vfloat4_arg a)
		{
			return Vfloat4(_mm_rsqrt_ps(a.Get128()));
		}

		inline Vfloat4 Rcp(Vfloat4_arg a)
		{
			return Vfloat4(_mm_rcp_ps(a.Get128()));
		}

		inline Vfloat LengthSqr(Vfloat4_arg a)
		{
			return Vfloat( _mm_dp_ps(a.Get128(), a.Get128(), 0xff) );
		}

		inline Vfloat Length(Vfloat4_arg a)
		{
			Vfloat lengthSqr = LengthSqr(a);
			return Sqrt(lengthSqr);
		}

		inline Vfloat4 Normalize(Vfloat4_arg a)
		{
			__m128 lengthSqrA = _mm_dp_ps(a.Get128(), a.Get128(), 0xff);

			static const _SiFloat128 kSmallValue = {{{0.00001f, 0.00001f, 0.00001f, 0.00001f}}};
			lengthSqrA = _mm_max_ps(lengthSqrA, kSmallValue); // 0割りerror対策.
			__m128 lengthA = _mm_sqrt_ps(lengthSqrA);

			return Vfloat4( _mm_div_ps(a.Get128(), lengthA) );
		}

		inline Vfloat4 NormalizeFast(Vfloat4_arg a)
		{
			Vfloat lengthSqr = LengthSqr(a);
			return Vfloat4(a * Rsqrt(lengthSqr)); // rsqrt精度良くない...
		}

	} // namespace Math
}
