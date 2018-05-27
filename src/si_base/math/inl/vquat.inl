#pragma once

#include "si_base/math/vquat.h"

#include <xmmintrin.h>
#include <smmintrin.h>
#include <math.h>
#include "si_base/math/vfloat.h"
#include "si_base/math/vfloat3.h"
#include "si_base/math/vfloat4.h"
#include "si_base/core/assert.h"
#include "si_base/math/math_internal.h"

namespace SI
{
	inline Vquat::Vquat()
		: m_v(kSiFloat128_0001)
	{
	}

	inline Vquat::Vquat(float x, float y, float z, float w)
		: m_v(_si_mm_set(x, y, z, w))
	{
	}
		
	inline Vquat::Vquat(Vfloat x, Vfloat y, Vfloat z, Vfloat w)
		: m_v(
			_mm_shuffle_ps(
				_mm_shuffle_ps(x.Get128(), y.Get128(), 0),
				_mm_shuffle_ps(z.Get128(), w.Get128(), 0),
				_MM_SHUFFLE(2,0,2,0))
		)
	{
	}
	
	inline Vquat::Vquat(Vfloat4 xyzw)
		: m_v(xyzw.Get128())
	{
	}
	
	inline Vquat::Vquat(Vfloat3 axis, float angle)
		: m_v( Math::CreateQuat(axis, angle).Get128() )
	{
	}
	
	inline Vquat::Vquat(Vfloat3 axis, Vfloat angle)
		: m_v( Math::CreateQuat(axis, angle).Get128() )
	{
	}
		
	inline Vquat::Vquat(const float* v)
		: m_v(_mm_load_ps(v))
	{
	}

	inline Vquat::Vquat(__m128 v)
		: m_v(v)
	{
	}
		
	inline void Vquat::SetX(Vfloat_arg x)
	{
		m_v = _mm_blend_ps(m_v, x.Get128(), 0b0001);
	}

	inline void Vquat::SetY(Vfloat_arg y)
	{
		__m128 _y = _mm_shuffle_ps(y.Get128(), y.Get128(), 0);
		m_v = _mm_blend_ps(m_v, _y, 0b0010);
	}
		
	inline void Vquat::SetZ(Vfloat_arg z)
	{
		__m128 _z = _mm_shuffle_ps(z.Get128(), z.Get128(), 0);
		m_v = _mm_blend_ps(m_v, _z, 0b0100);
	}

	inline void Vquat::SetW(Vfloat_arg w)
	{
		__m128 _w = _mm_shuffle_ps(w.Get128(), w.Get128(), 0);
		m_v = _mm_blend_ps(m_v, _w, 0b1000);
	}

	inline void Vquat::SetElement(uint32_t elementIndex, Vfloat_arg e)
	{
		__m128 _e = _mm_shuffle_ps(e.Get128(), e.Get128(), 0);
		m_v = _si_mm_blend(m_v, _e, (int)(1<<elementIndex));
	}

	inline void Vquat::Set(float value)
	{
		m_v = _mm_set1_ps(value);
	}

	inline void Vquat::Set(Vfloat_arg value)
	{
		m_v = _mm_shuffle_ps(value.Get128(), value.Get128(), 0);
	}
		
	inline void Vquat::Set(const float* v)
	{
		m_v = _mm_load_ps(v);
	}

	inline void Vquat::Set(float x, float y, float z, float w)
	{
		m_v = _si_mm_set(x, y, z, w);
	}

	inline void Vquat::Set(Vfloat_arg x, Vfloat_arg y, Vfloat_arg z, Vfloat_arg w)
	{
		__m128 xxyy = _mm_shuffle_ps(x.Get128(), y.Get128(), _MM_SHUFFLE(0,0,0,0));
		__m128 zzww = _mm_shuffle_ps(z.Get128(), w.Get128(), _MM_SHUFFLE(0,0,0,0));

		m_v = _mm_shuffle_ps(xxyy, zzww, _MM_SHUFFLE(2,0,2,0));
	}
	
	inline Vfloat Vquat::X() const
	{
		return Vfloat(m_v);
	}

	inline Vfloat Vquat::Y() const
	{
		return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(1,1,1,1)) );
	}

	inline Vfloat Vquat::Z() const
	{
		return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(2,2,2,2)) );
	}

	inline Vfloat Vquat::W() const
	{
		return Vfloat( _mm_shuffle_ps(m_v, m_v, _MM_SHUFFLE(3,3,3,3)) );
	}
		
	inline Vfloat Vquat::GetElement(uint32_t elementIndex) const
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

	inline Vquat Vquat::Normalize() const
	{
		return Math::Normalize(*this);
	}

	inline Vquat Vquat::Conjugate() const
	{
		return Math::Conjugate(*this);
	}

	inline Vquat Vquat::Inverse() const
	{
		return Math::Inverse(*this);
	}

	inline Vfloat Vquat::operator[](size_t i) const && // [] operatorは代入を許可しないようにしておく.
	{
		return GetElement((uint32_t)i);
	}
	
	inline Vquat Vquat::operator-() const
	{
		return Vquat( _mm_sub_ps( kSiFloat128_0000, m_v ) );
	}

	inline Vquat Vquat::operator*(Vquat_arg q)
	{
		return Math::Multiply(*this, q);
	}

	inline __m128 Vquat::Get128() const
	{
		return m_v;
	}

	namespace Math
	{
		inline Vquat Normalize (Vquat_arg q)
		{
			__m128 lengthSqr = _mm_dp_ps(q.Get128(), q.Get128(), 0xff);
			lengthSqr = _mm_max_ss(lengthSqr, _mm_set_ss(0.00001f)); // 0割りerror対策.

			__m128 lengthRsqr = _mm_rsqrt_ss(lengthSqr);
			lengthRsqr = _mm_shuffle_ps(lengthRsqr, lengthRsqr, 0);

			return Vquat(_mm_mul_ps(q.Get128(), lengthRsqr));
		}

		inline Vquat Conjugate(Vquat_arg q)
		{
			static const _SiFloat128 kConj = {{{-1.0f, -1.0f, -1.0f, 1.0f}}};
			return Vquat(_mm_mul_ps(q.Get128(), kConj));
		}

		inline Vquat Inverse(Vquat_arg q)
		{
			return q.Normalize().Conjugate();
		}
		
		inline Vquat CreateQuat(Vfloat3_arg v, Vfloat_arg r)
		{
			return CreateQuat(v, r.AsFloat());
		}

		inline Vquat CreateQuat(Vfloat3_arg v, float r)
		{
			float sinHalfR = sinf(0.5f * r);
			float cosHalfR = cosf(0.5f * r);

			Vfloat3 vn = sinHalfR * v.Normalize();

			__m128 w = _mm_set1_ps(cosHalfR);
			return Vquat( _mm_blend_ps(vn.Get128(), w, 0b1000) );
		}
				
		inline Vquat Multiply(Vquat_arg a, Vquat_arg b)
		{
			//      tmp0    tmp1    tmp2    tmp3
			// x' = ax*bw + aw*bx - ay*bz + az*by
			// y' = ay*bw + aw*by - az*bx + ax*bz
			// z' = az*bw + aw*bz - ax*by + ay*bx
			// w' = aw*bw - ax*bx - ay*by - az*bz
			__m128 axyzw = a.Get128();
			__m128 bwwww = _mm_shuffle_ps(b.Get128(), b.Get128(), _MM_SHUFFLE(3,3,3,3));
			__m128 tmp0  = _mm_mul_ps(axyzw, bwwww);
			
			__m128 awwwx = _mm_shuffle_ps(a.Get128(), a.Get128(), _MM_SHUFFLE(0,3,3,3));
			__m128 bxyzx = _mm_shuffle_ps(b.Get128(), b.Get128(), _MM_SHUFFLE(0,2,1,0));
			__m128 tmp1  = _mm_mul_ps(awwwx, bxyzx);
			static const _SiFloat128 k111_1 = {{{1.0f, 1.0f, 1.0f, -1.0f}}};
			tmp1 = _mm_mul_ps( tmp1, k111_1 );
			
			__m128 ayzxy = _mm_shuffle_ps(a.Get128(), a.Get128(), _MM_SHUFFLE(1,0,2,1));
			__m128 bzxyy = _mm_shuffle_ps(b.Get128(), b.Get128(), _MM_SHUFFLE(1,1,0,2));
			__m128 tmp2  = _mm_mul_ps(ayzxy, bzxyy);
			
			__m128 azxyz = _mm_shuffle_ps(a.Get128(), a.Get128(), _MM_SHUFFLE(2,1,0,2));
			__m128 byzxz = _mm_shuffle_ps(b.Get128(), b.Get128(), _MM_SHUFFLE(2,0,2,1));
			__m128 tmp3  = _mm_mul_ps(azxyz, byzxz);
			tmp3 = _mm_mul_ps( tmp3, k111_1 );

			__m128 ret = _mm_add_ps(tmp0, tmp1);
			ret = _mm_sub_ps(ret, tmp2);
			ret = _mm_add_ps(ret, tmp3);

			return Vquat(ret);
		}
				
		inline Vfloat3 Multiply(Vfloat3_arg v, Vquat_arg q)
		{
			Vquat vq = Vquat(_mm_blend_ps(v.Get128(), kSiFloat128_0000, 0b1000));

			Vquat qc = q.Inverse();
			Vquat ret = qc * vq * q;

			return Vfloat3(ret.Get128());
		}

	} // namespace Math
}
