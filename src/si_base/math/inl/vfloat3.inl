#pragma once

#include <smmintrin.h>

#include "si_base/math/vfloat3.h"

#include "si_base/math/vfloat4.h"
#include "si_base/math/vfloat.h"
#include "si_base/core/assert.h"


namespace SI
{
	inline Vfloat3::Vfloat3()
		: m_v(kSiFloat128_0000)
	{
	}
		
	inline Vfloat3::Vfloat3(float x, float y, float z)
		: m_v(_si_mm_set(x, y, z, 0.0f))
	{
	}
		
	inline Vfloat3::Vfloat3(Vfloat x, Vfloat y, Vfloat z)
		: m_v(
			_mm_shuffle_ps(
				_mm_shuffle_ps(x.Get128(), y.Get128(), _MM_SHUFFLE(0,0,0,0)),
				_mm_shuffle_ps(z.Get128(), z.Get128(), _MM_SHUFFLE(0,0,0,0)),
				_MM_SHUFFLE(2,0,2,0))
		)
	{
	}
	
	inline Vfloat3::Vfloat3(float value)
		: m_v(_mm_set1_ps(value))
	{
	}
	
	inline Vfloat3::Vfloat3(Vfloat_arg value)
		: m_v(_mm_shuffle_ps(value.Get128(), value.Get128(), 0))
	{
	}
	
	inline Vfloat3::Vfloat3(const float* v)
		: m_v(_si_mm_set(v[0], v[1], v[2], 0.0f))
	{
	}

	inline Vfloat3::Vfloat3(__m128 v)
		: m_v(v)
	{
	}

	inline void Vfloat3::SetX(Vfloat_arg x)
	{
		m_v = _mm_blend_ps(m_v, x.Get128(), 0b0001);
	}

	inline void Vfloat3::SetY(Vfloat_arg y)
	{
		__m128 _y = _mm_shuffle_ps(y.Get128(), y.Get128(), 0);
		m_v = _mm_blend_ps(m_v, _y, 0b0010);
	}
		
	inline void Vfloat3::SetZ(Vfloat_arg z)
	{
		__m128 _z = _mm_shuffle_ps(z.Get128(), z.Get128(), 0);
		m_v = _mm_blend_ps(m_v, _z, 0b0100);
	}

	inline void Vfloat3::SetElement(uint32_t elementIndex, Vfloat_arg e)
	{
		SI_ASSERT(elementIndex<3);
		__m128 _e = _mm_shuffle_ps(e.Get128(), e.Get128(), 0);
		m_v = _si_mm_blend(m_v, _e, 1<<elementIndex);
	}

	inline void Vfloat3::Set(float value)
	{
		m_v = _mm_set1_ps(value);
	}

	inline void Vfloat3::Set(Vfloat_arg value)
	{
		m_v = _mm_shuffle_ps(value.Get128(), value.Get128(), 0);
	}
		
	inline void Vfloat3::Set(const float* v)
	{
		m_v = _si_mm_set(v[0], v[1], v[2], 0.0f);
	}

	inline void Vfloat3::Set(float x, float y, float z)
	{
		m_v = _si_mm_set(x, y, z, 0.0f);
	}

	inline void Vfloat3::Set(Vfloat_arg x, Vfloat_arg y, Vfloat_arg z)
	{
		__m128 xxyy = _mm_shuffle_ps(x.Get128(), y.Get128(), _MM_SHUFFLE(0,0,0,0));
		__m128 zzzz = _mm_shuffle_ps(z.Get128(), z.Get128(), _MM_SHUFFLE(0,0,0,0));

		m_v = _mm_shuffle_ps(xxyy, zzzz, _MM_SHUFFLE(2,0,2,0));
	}

	inline Vfloat Vfloat3::X() const
	{
		return Vfloat(m_v);
	}

	inline Vfloat Vfloat3::Y() const
	{
		return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(1,1,1,1)) );
	}

	inline Vfloat Vfloat3::Z() const
	{
		return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(2,2,2,2)) );
	}
	
	inline float Vfloat3::Xf() const
	{
		return X().AsFloat();
	}

	inline float Vfloat3::Yf() const
	{
		return Y().AsFloat();
	}

	inline float Vfloat3::Zf() const
	{
		return Z().AsFloat();
	}

	inline Vfloat Vfloat3::GetElement(uint32_t elementIndex) const
	{
		SI_ASSERT(elementIndex<3);
		switch(elementIndex)
		{
		case 0:
			return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(0,0,0,0)) );
		case 1:
			return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(1,1,1,1)) );
		case 2:
			return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(2,2,2,2)) );
		}

		return Vfloat(kSiFloat128_0000);
	}
	template<uint32_t xIndex, uint32_t yIndex, uint32_t zIndex>
	inline Vfloat3 Vfloat3::Swizzle() const
	{
		return Vfloat3( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(0, zIndex, yIndex, xIndex)) );
	}

	inline Vfloat3 Vfloat3::Swizzle(uint32_t xIndex, uint32_t yIndex, uint32_t zIndex) const
	{
		__m128 xyzwArray[3] = 
		{
			m_v,
			_mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(1,1,1,1)),
			_mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(2,2,2,2)),
		};

		return Vfloat3(
			_mm_shuffle_ps(
				_mm_shuffle_ps(xyzwArray[xIndex], xyzwArray[yIndex], 0),
				_mm_shuffle_ps(xyzwArray[zIndex], kSiFloat128_0000, 0),
				_MM_SHUFFLE(2,0,2,0)));
	}

	inline Vfloat4 Vfloat3::XYZ0() const
	{
		Vfloat4(_mm_blend_ps(m_v, kSiFloat128_0000, 0b1000));
	}

	inline Vfloat4 Vfloat3::XYZ1() const
	{
		Vfloat4(_mm_blend_ps(m_v, kSiFloat128_1111, 0b1000));
	}

	inline Vfloat Vfloat3::LengthSqr() const
	{
		return Math::LengthSqr(*this);
	}

	inline Vfloat Vfloat3::Length() const
	{
		return Math::Length(*this);
	}
	
	inline Vfloat3 Vfloat3::Normalize() const
	{
		return Math::Normalize(*this);
	}
	
	inline Vfloat3 Vfloat3::NormalizeFast() const
	{
		return Math::NormalizeFast(*this);
	}
		
	inline PackedFloat3 Vfloat3::GetPackedFloat3() const
	{
		PackedFloat3 p;
		p.m_v[0] = X().AsFloat();
		p.m_v[1] = Y().AsFloat();
		p.m_v[2] = Z().AsFloat();
	}

	inline Vfloat3 Vfloat3::operator-() const
	{
		return Vfloat3( _mm_sub_ps( kSiFloat128_0000, Get128() ) );
	}

	inline const Vfloat Vfloat3::operator[](size_t i) const // [] operatorは代入を許可しないようにしておく.
	{
		return GetElement((uint32_t)i);
	}

	inline Vfloat3 Vfloat3::operator+(Vfloat3_arg v) const
	{
		return Vfloat3(_mm_add_ps(m_v, v.m_v));
	}

	inline Vfloat3 Vfloat3::operator-(Vfloat3_arg v) const
	{
		return Vfloat3(_mm_sub_ps(m_v, v.m_v));
	}

	inline Vfloat3 Vfloat3::operator*(Vfloat3_arg v) const
	{
		return Vfloat3(_mm_mul_ps(m_v, v.m_v));
	}

	inline Vfloat3 Vfloat3::operator/(Vfloat3_arg v) const
	{
		return Vfloat3(_mm_div_ps(m_v, v.m_v));
	}

	inline Vfloat3 Vfloat3::operator*(Vfloat_arg f) const
	{
		return (*this) * Vfloat3(f);
	}

	inline Vfloat3 Vfloat3::operator/(Vfloat_arg f) const
	{
		return (*this) / Vfloat3(f);
	}

	inline Vfloat3 Vfloat3::operator*(float f) const
	{
		return (*this) * Vfloat3(f);
	}

	inline Vfloat3 Vfloat3::operator/(float f) const
	{
		return (*this) / Vfloat3(f);
	}
	
	inline Vfloat3 Vfloat3::operator*(Vfloat4x3_arg m) const
	{
		return Math::Multiply(*this, m);
	}
	
	inline Vfloat3 Vfloat3::operator*(Vquat_arg q) const
	{
		return Math::Multiply(*this, q);
	}

	inline Vfloat3& Vfloat3::operator+=(Vfloat3_arg v)
	{
		*this = *this + v;
		return (*this);
	}

	inline Vfloat3& Vfloat3::operator-=(Vfloat3_arg v)
	{
		*this = *this - v;
		return (*this);
	}

	inline bool Vfloat3::operator==(const Vfloat3& v) const
	{
		__m128 cmp = _mm_cmpeq_ps(m_v, v.m_v);
		uint16_t mask = (uint16_t)_mm_movemask_epi8(_mm_castps_si128(cmp));
		return ((mask&0x0fff) == 0x0fff);
	}

	inline bool Vfloat3::operator!=(const Vfloat3& v) const
	{
		return !((*this)==v);
	}
	
	inline Vfloat3 Vfloat3::Zero()
	{
		return Vfloat3(kSiFloat128_0000.m_v);
	}

	inline Vfloat3 Vfloat3::One()
	{
		return Vfloat3(kSiFloat128_1111.m_v);
	}

	inline Vfloat3 Vfloat3::AxisX()
	{
		return Vfloat3(kSiFloat128_1000.m_v);
	}

	inline Vfloat3 Vfloat3::AxisY()
	{
		return Vfloat3(kSiFloat128_0100.m_v);
	}

	inline Vfloat3 Vfloat3::AxisZ()
	{
		return Vfloat3(kSiFloat128_0010.m_v);
	}
	
	inline __m128 Vfloat3::Get128() const
	{
		return m_v;
	}

	//////////////////////////////////////////////////////////////////////
		
	inline Vfloat3 operator*(Vfloat f, Vfloat3_arg v )
	{
		return Vfloat3(f) * v;
	}
		
	inline Vfloat3 operator/(Vfloat f, Vfloat3_arg v )
	{
		return Vfloat3(f) / v;
	}

	inline Vfloat3 operator*(float f, Vfloat3_arg v)
	{
		return Vfloat3(f) * v;
	}

	inline Vfloat3 operator/(float f, Vfloat3_arg v)
	{
		return Vfloat3(f) / v;
	}
	
	namespace Math
	{
		inline Vfloat3 Min(Vfloat3_arg a, Vfloat3_arg b)
		{
			return Vfloat3(_mm_min_ps(a.Get128(), b.Get128()));
		}

		inline Vfloat3 Max(Vfloat3_arg a, Vfloat3_arg b)
		{
			return Vfloat3(_mm_max_ps(a.Get128(), b.Get128()));
		}

		inline Vfloat3 Abs(Vfloat3_arg a)
		{
			return Vfloat3(_mm_and_ps(a.Get128(), kSiUint128_AbsMask));
		}

		inline Vfloat3 Sqrt(Vfloat3_arg a)
		{
			return Vfloat3(_mm_sqrt_ps(a.Get128()));
		}

		inline Vfloat3 Rsqrt(Vfloat3_arg a)
		{
			return Vfloat3(_mm_rsqrt_ps(a.Get128()));
		}

		inline Vfloat3 Rcp(Vfloat3_arg a)
		{
			return Vfloat3(_mm_rcp_ps(a.Get128()));
		}
		
		inline Vfloat Dot(Vfloat3_arg a, Vfloat3_arg b)
		{
			return Vfloat(_mm_dp_ps( a.Get128(), b.Get128(), 0b01111111)); // 0b01111111 = w無視したdot
		}

		inline Vfloat3 Cross(Vfloat3_arg a, Vfloat3_arg b)
		{
			__m128 yzxw0 = _mm_shuffle_ps(a.Get128(), a.Get128(), _MM_SHUFFLE(3, 0, 2, 1));
			__m128 zxyw1 = _mm_shuffle_ps(b.Get128(), b.Get128(), _MM_SHUFFLE(3, 1, 0, 2));

			__m128 p = _mm_mul_ps(yzxw0, zxyw1);
		
			__m128 zxyw0 = _mm_shuffle_ps(a.Get128(), a.Get128(), _MM_SHUFFLE(3, 1, 0, 2));
			__m128 yzxw1 = _mm_shuffle_ps(b.Get128(), b.Get128(), _MM_SHUFFLE(3, 0, 2, 1));
		
			__m128 m = _mm_mul_ps(zxyw0, yzxw1);

			return Vfloat3(_mm_sub_ps(p, m));
		}

		inline Vfloat LengthSqr(Vfloat3_arg a)
		{
			return Vfloat( _mm_dp_ps(a.Get128(), a.Get128(), 0b01111111) ); // 0b01111111 = w無視したdot
		}

		inline Vfloat Length   (Vfloat3_arg a)
		{
			Vfloat lengthSqr = LengthSqr(a);
			return Sqrt(lengthSqr);
		}

		inline Vfloat3 Normalize (Vfloat3_arg a)
		{
			__m128 lengthSqrA = _mm_dp_ps(a.Get128(), a.Get128(), 0b01111111);

			static const _SiFloat128 kSmallValue = {{{0.00001f, 0.00001f, 0.00001f, 0.00001f}}};
			lengthSqrA = _mm_max_ps(lengthSqrA, kSmallValue); // 0割りerror対策.
			__m128 lengthA = _mm_sqrt_ps(lengthSqrA);

			return Vfloat3( _mm_div_ps(a.Get128(), lengthA) );
		}

		inline Vfloat3 NormalizeFast(Vfloat3_arg a)
		{
			Vfloat lengthSqr = LengthSqr(a);
			return Vfloat3(a * Rsqrt(lengthSqr)); // rsqrt精度良くない...
		}

	} // namespace Math
}
