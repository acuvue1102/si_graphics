#pragma once

#include "si_base/math/vfloat4x4.h"

#include <cstdint>
#include <xmmintrin.h>
#include <smmintrin.h>
#include "si_base/core/assert.h"
#include "si_base/math/vfloat.h"
#include "si_base/math/vfloat4.h"
#include "si_base/math/vfloat3x3.h"
#include "si_base/math/vfloat4x3.h"

namespace SI
{
	inline Vfloat4x4::Vfloat4x4()
	{
		m_row[0] = kSiFloat128_1000;
		m_row[1] = kSiFloat128_0100;
		m_row[2] = kSiFloat128_0010;
		m_row[3] = kSiFloat128_0001;
	}
	
	inline Vfloat4x4::Vfloat4x4(const Vfloat4x4& m)
	{
		m_row[0] = m.m_row[0];
		m_row[1] = m.m_row[1];
		m_row[2] = m.m_row[2];
		m_row[3] = m.m_row[3];
	}

	inline Vfloat4x4::Vfloat4x4(
		Vfloat4_arg row0,
		Vfloat4_arg row1,
		Vfloat4_arg row2,
		Vfloat4_arg row3)
	{
		m_row[0] = row0.Get128();
		m_row[1] = row1.Get128();
		m_row[2] = row2.Get128();
		m_row[3] = row3.Get128();
	}
	
	inline Vfloat4x4::Vfloat4x4(
		Vfloat4x3_arg m)
	{
		const __m128 zero = kSiFloat128_0000;
		m_row[0] = _mm_blend_ps(m.GetRow128(0), zero, 0b1000);
		m_row[1] = _mm_blend_ps(m.GetRow128(1), zero, 0b1000);
		m_row[2] = _mm_blend_ps(m.GetRow128(2), zero, 0b1000);
		m_row[3] = _mm_blend_ps(m.GetRow128(3), kSiFloat128_1111, 0b1000);
	}

	inline Vfloat4x4::Vfloat4x4(
		Vfloat3x3_arg rot,
		Vfloat3_arg trans)
	{
		const __m128 zero = kSiFloat128_0000;
		m_row[0] = _mm_blend_ps(rot.GetRow128(0), zero, 0b1000);
		m_row[1] = _mm_blend_ps(rot.GetRow128(1), zero, 0b1000);
		m_row[2] = _mm_blend_ps(rot.GetRow128(2), zero, 0b1000);
		m_row[3] = _mm_blend_ps(trans.Get128(), kSiFloat128_1111, 0b1000);
	}

	inline Vfloat4x4::Vfloat4x4(
		__m128 row0,
		__m128 row1,
		__m128 row2,
		__m128 row3)
	{
		m_row[0] = row0;
		m_row[1] = row1;
		m_row[2] = row2;
		m_row[3] = row3;
	}

	inline Vfloat4x4::Vfloat4x4(const float* m)
	{
		m_row[0] = _mm_load_ps(m);
		m_row[1] = _mm_load_ps(&m[4]);
		m_row[2] = _mm_load_ps(&m[8]);
		m_row[3] = _mm_load_ps(&m[12]);
	}

	inline void Vfloat4x4::SetRow(uint32_t rowIndex, Vfloat4_arg row)
	{
		SI_ASSERT(rowIndex<4);
		m_row[rowIndex] = row.Get128();
	}

	inline void Vfloat4x4::SetColumn(uint32_t columnIndex, Vfloat4_arg column)
	{
		SI_ASSERT(columnIndex<4);
		__m128 mask = _si_mm_get_mask(1<<columnIndex);
		m_row[0] = _si_mm_blend(m_row[0], column.Get128(), mask);
		m_row[1] = _si_mm_blend(m_row[1], column.Get128(), mask);
		m_row[2] = _si_mm_blend(m_row[2], column.Get128(), mask);
		m_row[3] = _si_mm_blend(m_row[3], column.Get128(), mask);
	}

	inline void Vfloat4x4::Set(uint32_t rowIndex, uint32_t columnIndex, Vfloat_arg element)
	{
		SI_ASSERT(rowIndex<4);
		SI_ASSERT(columnIndex<4);

		uint32_t mask = 1<<columnIndex;
		__m128 element0000 = _mm_shuffle_ps(element.Get128(), element.Get128(), 0x00); // element.xxxx
		m_row[rowIndex] = _si_mm_blend(m_row[rowIndex], element0000, mask);
	}
	
	inline void Vfloat4x4::SetUpper3x3(Vfloat3x3_arg m)
	{
		const __m128 zero = kSiFloat128_0000;
		m_row[0] = _mm_blend_ps(m.GetRow128(0), zero, 0b1000);
		m_row[1] = _mm_blend_ps(m.GetRow128(1), zero, 0b1000);
		m_row[2] = _mm_blend_ps(m.GetRow128(2), zero, 0b1000);
	}

	inline Vfloat4 Vfloat4x4::GetRow(uint32_t rowIndex) const
	{
		SI_ASSERT(rowIndex<4);
		return Vfloat4(m_row[rowIndex]);
	}

	inline Vfloat4 Vfloat4x4::GetColumn(uint32_t columnIndex) const
	{
		SI_ASSERT(columnIndex<4);
		switch(columnIndex)
		{
		case 0:
			return Vfloat4(
				_mm_unpacklo_ps(
					_mm_shuffle_ps(m_row[0], m_row[2], _MM_SHUFFLE(0,0,0,0)),
					_mm_shuffle_ps(m_row[1], m_row[3], _MM_SHUFFLE(0,0,0,0))));
		case 1:
			return Vfloat4(
				_mm_unpacklo_ps(
					_mm_shuffle_ps(m_row[0], m_row[2], _MM_SHUFFLE(1,1,1,1)),
					_mm_shuffle_ps(m_row[1], m_row[3], _MM_SHUFFLE(1,1,1,1))));
		case 2:
			return Vfloat4(
				_mm_unpacklo_ps(
					_mm_shuffle_ps(m_row[0], m_row[2], _MM_SHUFFLE(2,2,2,2)),
					_mm_shuffle_ps(m_row[1], m_row[3], _MM_SHUFFLE(2,2,2,2))));
		case 3:
			return Vfloat4(
				_mm_unpacklo_ps(
					_mm_shuffle_ps(m_row[0], m_row[2], _MM_SHUFFLE(3,3,3,3)),
					_mm_shuffle_ps(m_row[1], m_row[3], _MM_SHUFFLE(3,3,3,3))));
		default:
			break;
		}

		return Vfloat4();
	}

	inline Vfloat Vfloat4x4::Get(uint32_t rowIndex, uint32_t columnIndex) const
	{
		SI_ASSERT(rowIndex<4);
		SI_ASSERT(columnIndex<4);

		switch(columnIndex)
		{
		case 0:
			return Vfloat(_mm_shuffle_ps(m_row[rowIndex], m_row[rowIndex], _MM_SHUFFLE(0,0,0,0)));
		case 1:
			return Vfloat(_mm_shuffle_ps(m_row[rowIndex], m_row[rowIndex], _MM_SHUFFLE(1,1,1,1)));
		case 2:
			return Vfloat(_mm_shuffle_ps(m_row[rowIndex], m_row[rowIndex], _MM_SHUFFLE(2,2,2,2)));
		case 3:
			return Vfloat(_mm_shuffle_ps(m_row[rowIndex], m_row[rowIndex], _MM_SHUFFLE(3,3,3,3)));
		}

		return Vfloat(0.0f);
	}
	
	inline Vfloat3x3 Vfloat4x4::GetUpper3x3() const
	{
		return Vfloat3x3(
			m_row[0],
			m_row[1],
			m_row[2]);
	}
	
	inline Vfloat4x4 Vfloat4x4::Multiply(Vfloat4x4_arg m) const
	{
		return Math::Multiply(*this, m);
	}

	inline Vfloat4x4 Vfloat4x4::Transpose() const
	{
		return Math::Transpose(*this);
	}
	
	inline Vfloat4x4& Vfloat4x4::operator=(const Vfloat4x4& m)
	{
		m_row[0] = m.m_row[0];
		m_row[1] = m.m_row[1];
		m_row[2] = m.m_row[2];
		m_row[3] = m.m_row[3];
		return (*this);
	}
	
	inline Vfloat4 Vfloat4x4::operator[](size_t i) const
	{
		SI_ASSERT(i<4);
		return Vfloat4(m_row[i]);
	}
	
	inline Vfloat4x4 Vfloat4x4::operator*(Vfloat4x4_arg m) const
	{
		return Math::Multiply(*this, m);
	}
	
	inline Vfloat4x4 Vfloat4x4::Translate(Vfloat3_arg translate)
	{
		return Math::Translate4x4(translate);
	}

	inline Vfloat4x4 Vfloat4x4::Scale(Vfloat3_arg scale)
	{
		return Math::Scale4x4(scale);
	}

	inline Vfloat4x4 Vfloat4x4::RotateX(float radian)
	{
		return Math::RotateX4x4(radian);
	}

	inline Vfloat4x4 Vfloat4x4::RotateY(float radian)
	{
		return Math::RotateY4x4(radian);
	}

	inline Vfloat4x4 Vfloat4x4::RotateZ(float radian)
	{
		return Math::RotateZ4x4(radian);
	}

	inline Vfloat4x4 Vfloat4x4::Identity()
	{
		return Vfloat4x4(
			kSiFloat128_1000,
			kSiFloat128_0100,
			kSiFloat128_0010,
			kSiFloat128_0001);
	}
	
	inline __m128 Vfloat4x4::GetRow128(uint32_t rowIndex) const
	{
		SI_ASSERT(rowIndex<4);
		return m_row[rowIndex];
	}
	
	inline Vfloat4 operator*(Vfloat4_arg v, Vfloat4x4_arg m)
	{
		return Math::Multiply(v, m);
	}
	
	inline Vfloat4x4 operator*(Vfloat4x4_arg m0, Vfloat4x3_arg m1)
	{
		return Math::Multiply(m0, m1);
	}

	inline Vfloat4x4 operator*(Vfloat4x3_arg m0, Vfloat4x4_arg m1)
	{
		return Math::Multiply(m0, m1);
	}
	
	namespace Math
	{
		inline Vfloat4x4 Multiply(Vfloat4x4_arg m0, Vfloat4x4_arg m1)
		{
			__m128 result[4];

			for(uint32_t i=0; i<4; ++i)
			{
				__m128 mi = m0.GetRow128(i);
				__m128 x = _mm_shuffle_ps(mi, mi, _MM_SHUFFLE(0,0,0,0));
				__m128 y = _mm_shuffle_ps(mi, mi, _MM_SHUFFLE(1,1,1,1));
				__m128 z = _mm_shuffle_ps(mi, mi, _MM_SHUFFLE(2,2,2,2));
				__m128 w = _mm_shuffle_ps(mi, mi, _MM_SHUFFLE(3,3,3,3));
			
				x = _mm_mul_ps(x, m1.GetRow128(0));
				y = _mm_mul_ps(y, m1.GetRow128(1));
				z = _mm_mul_ps(z, m1.GetRow128(2));
				w = _mm_mul_ps(w, m1.GetRow128(3));

				result[i] = _mm_add_ps(_mm_add_ps(x, y), _mm_add_ps(z, w));
			}

			return Vfloat4x4(
				result[0],
				result[1],
				result[2],
				result[3]);
		}

		inline Vfloat4x4 Multiply(Vfloat4x4_arg m0, Vfloat4x3_arg m1)
		{
			return Multiply(m0, Vfloat4x4(m1));
		}

		inline Vfloat4x4 Multiply(Vfloat4x3_arg m0, Vfloat4x4_arg m1)
		{
			return Multiply(Vfloat4x4(m0), m1);
		}

		inline Vfloat4 Multiply(Vfloat4_arg v, Vfloat4x4_arg m)
		{
			__m128 vi = v.Get128();
			__m128 x = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(0,0,0,0));
			__m128 y = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(1,1,1,1));
			__m128 z = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(2,2,2,2));
			__m128 w = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(3,3,3,3));
			
			x = _mm_mul_ps(x, m.GetRow128(0));
			y = _mm_mul_ps(y, m.GetRow128(1));
			z = _mm_mul_ps(z, m.GetRow128(2));
			w = _mm_mul_ps(w, m.GetRow128(3));

			Vfloat4(_mm_add_ps(_mm_add_ps(x, y), _mm_add_ps(z, w)));
		}

		inline Vfloat3 Multiply(Vfloat3_arg v, Vfloat4x4_arg m)
		{
			__m128 vi = v.Get128();
			__m128 x = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(0,0,0,0));
			__m128 y = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(1,1,1,1));
			__m128 z = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(2,2,2,2));
			
			       x = _mm_mul_ps(x, m.GetRow128(0));
			       y = _mm_mul_ps(y, m.GetRow128(1));
			       z = _mm_mul_ps(z, m.GetRow128(2));
			__m128 w = m.GetRow128(3);

			Vfloat4(_mm_add_ps(_mm_add_ps(x, y), _mm_add_ps(z, w)));
		}

		inline Vfloat4x4 Transpose(Vfloat4x4_arg m)
		{
			__m128 tmp0 = _mm_shuffle_ps((m.GetRow128(0)), (m.GetRow128(1)), _MM_SHUFFLE(1,0,1,0));
			__m128 tmp1 = _mm_shuffle_ps((m.GetRow128(0)), (m.GetRow128(1)), _MM_SHUFFLE(3,2,3,2));
			__m128 tmp2 = _mm_shuffle_ps((m.GetRow128(2)), (m.GetRow128(3)), _MM_SHUFFLE(1,0,1,0));
			__m128 tmp3 = _mm_shuffle_ps((m.GetRow128(2)), (m.GetRow128(3)), _MM_SHUFFLE(3,2,3,2));

			return Vfloat4x4(
				_mm_shuffle_ps(tmp0, tmp2, _MM_SHUFFLE(2,0,2,0)),
				_mm_shuffle_ps(tmp0, tmp2, _MM_SHUFFLE(3,1,3,1)),
				_mm_shuffle_ps(tmp1, tmp3, _MM_SHUFFLE(2,0,2,0)),
				_mm_shuffle_ps(tmp1, tmp3, _MM_SHUFFLE(3,1,3,1)));
		}
		
		inline Vfloat4x4 Perspective(float width, float height, float nearPlane, float farPlane)
		{
			float near2 = nearPlane + nearPlane;
			float farNear = (farPlane - nearPlane);
			
			// near2/height, 0.0f,          0.0f,               0.0f
			// 0.0f,         near2/height,  0.0f,               0.0f
			// 0.0f,         0.0f,          far/farNear,        1.0f
			// 0.0f,         0.0f,          -near*far/farNear,  0.0f
			
			__m128 values = _si_mm_set(near2/width, near2/height, farPlane/farNear, -nearPlane*farPlane/farNear);
			__m128 r0 = _mm_mul_ps(values, kSiFloat128_1000);
			__m128 r1 = _mm_mul_ps(values, kSiFloat128_0100);
			__m128 r2 = _mm_mul_ps(values, kSiFloat128_0010);
			r2 = _mm_add_ps(r2, kSiFloat128_0001);
			__m128 r3 = _mm_mul_ps(values, kSiFloat128_0001);
			r3 = _mm_shuffle_ps(r3, r3, _MM_SHUFFLE(0, 3, 0, 0));

			return Vfloat4x4(r0, r1, r2, r3);
		}		
		
		inline Vfloat4x4 Ortho(float width, float height, float nearPlane, float farPlane)
		{
			float near2 = nearPlane + nearPlane;
			float farNear = (farPlane - nearPlane);
			
			// 2.0f/height,  0.0f,          0.0f,               0.0f
			// 0.0f,         2.0f/height,   0.0f,               0.0f
			// 0.0f,         0.0f,          1.0f/farNear,       1.0f
			// 0.0f,         0.0f,          -near/farNear,      0.0f
			
			__m128 values = _si_mm_set(2.0f/width, 2.0f/height, 1.0f/farNear, -nearPlane/farNear);
			__m128 r0 = _mm_mul_ps(values, kSiFloat128_1000);
			__m128 r1 = _mm_mul_ps(values, kSiFloat128_0100);
			__m128 r2 = _mm_mul_ps(values, kSiFloat128_0010);
			r2 = _mm_add_ps(r2, kSiFloat128_0001);
			__m128 r3 = _mm_mul_ps(values, kSiFloat128_0001);
			r3 = _mm_shuffle_ps(r3, r3, _MM_SHUFFLE(0, 3, 0, 0));

			return Vfloat4x4(r0, r1, r2, r3);
		}

		inline Vfloat4x4 Translate4x4(Vfloat3_arg translate)
		{
			return Vfloat4x4(
				kSiFloat128_1000,
				kSiFloat128_0100,
				kSiFloat128_0010,
				_mm_blend_ps(translate.Get128(), kSiFloat128_1111, 0b1000) );
		}

		inline Vfloat4x4 Scale4x4(Vfloat3_arg scale)
		{
			return Vfloat4x4(
				_mm_mul_ps( scale.Get128(), kSiFloat128_1000 ),
				_mm_mul_ps( scale.Get128(), kSiFloat128_0100 ),
				_mm_mul_ps( scale.Get128(), kSiFloat128_0010 ),
				kSiFloat128_0001);
		}

		inline Vfloat4x4 RotateX4x4(float radian)
		{
			float s = sinf(radian);
			float c = cosf(radian);
			__m128 scsc = _si_mm_set(s,c,s,c);
			__m128 cscs = _mm_shuffle_ps(scsc, scsc, _MM_SHUFFLE(0, 1, 0, 1));
			
			static const _SiFloat128 kSiFloat128_0110  = {{{ 0.0f,  1.0f, 1.0f, 0.0f }}};
			static const _SiFloat128 kSiFloat128_0_110 = {{{ 0.0f, -1.0f, 1.0f, 0.0f }}};

			return Vfloat4x4(
				kSiFloat128_1000,
				_mm_mul_ps( scsc, kSiFloat128_0110),
				_mm_mul_ps( cscs, kSiFloat128_0_110),
				kSiFloat128_0001);
		}

		inline Vfloat4x4 RotateY4x4(float radian)
		{
			float s = sinf(radian);
			float c = cosf(radian);
			__m128 sscc = _si_mm_set(s,s,c,c);
			__m128 ccss = _mm_shuffle_ps(sscc, sscc, _MM_SHUFFLE(0, 0, 2, 2));
			
			static const _SiFloat128 kSiFloat128_10_10 = {{{  1.0f, 0.0f,-1.0f, 0.0f }}};
			static const _SiFloat128 kSiFloat128_1010  = {{{  1.0f, 0.0f, 1.0f, 0.0f }}};

			return Vfloat4x4(
				_mm_mul_ps( ccss, kSiFloat128_10_10),
				kSiFloat128_0100,
				_mm_mul_ps( sscc, kSiFloat128_1010),
				kSiFloat128_0001);
		}

		inline Vfloat4x4 RotateZ4x4(float radian)
		{
			float s = sinf(radian);
			float c = cosf(radian);
			__m128 scsc = _si_mm_set(s,c,s,c);
			__m128 cscs = _mm_shuffle_ps(scsc, scsc, _MM_SHUFFLE(0, 1, 0, 1));
			
			static const _SiFloat128 kSiFloat128_1100  = {{{  1.0f, 1.0f, 0.0f, 0.0f }}};
			static const _SiFloat128 kSiFloat128__1100 = {{{ -1.0f, 1.0f, 0.0f, 0.0f }}};

			return Vfloat4x4(
				_mm_mul_ps( cscs, kSiFloat128_1100),
				_mm_mul_ps( scsc, kSiFloat128__1100),
				kSiFloat128_0010,
				kSiFloat128_0001);
		}
	}

} // namespace SI
