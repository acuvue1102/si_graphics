#pragma once

#include "si_base/math/Vfloat3x3.h"

#include <stdint.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include "si_base/core/assert.h"
#include "si_base/math/vfloat.h"
#include "si_base/math/vfloat4.h"
#include "si_base/math/vfloat4x4.h"
#include "si_base/math/vquat.h"

namespace SI
{
	inline Vfloat3x3::Vfloat3x3()
	{
		m_row[0] = kSiFloat128_1000;
		m_row[1] = kSiFloat128_0100;
		m_row[2] = kSiFloat128_0010;
		m_row[3] = kSiFloat128_0001;
	}

	inline Vfloat3x3::Vfloat3x3(
		Vfloat3_arg row0,
		Vfloat3_arg row1,
		Vfloat3_arg row2)
	{
		m_row[0] = row0.Get128();
		m_row[1] = row1.Get128();
		m_row[2] = row2.Get128();
	}

	inline Vfloat3x3::Vfloat3x3(
		Vquat_arg q)
	{
		// 1.0f-2*qy*qy-2*qz*qz;      2*qx*qy+2*qw*qz;      2*qx*qz-2*qw*qy;		
		{
			static const _SiFloat128 kSiFloat128__2220 = {{{-2.0f, 2.0f, 2.0f, 0.0f}}};
			__m128 yxx = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 0, 0, 1));
			__m128 yyz = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 2, 1, 1));
			__m128 tmp0 = _mm_mul_ps(kSiFloat128__2220, _mm_mul_ps(yxx, yyz));
			
			static const _SiFloat128 kSiFloat128__22_20 = {{{-2.0f, 2.0f, -2.0f, 0.0f}}};
			__m128 zww = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 3, 3, 2));
			__m128 zzy = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 1, 2, 2));
			__m128 tmp1 = _mm_mul_ps(kSiFloat128__22_20, _mm_mul_ps(zww, zzy));
			
			// (1,0,0) + tmp0 + tmp1
			tmp0 = _mm_add_ps(tmp0, tmp1);
			m_row[0] = _mm_add_ps(kSiFloat128_1000, tmp0);
		}
		
		//      2*qx*qy-2*qw*qz; 1.0f-2*qx*qx-2*qz*qz;      2*qy*qz+2*qw*qx;
		{
			static const _SiFloat128 kSiFloat128_2_220 = {{{2.0f, -2.0f, 2.0f, 0.0f}}};
			__m128 xxy = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 1, 0, 0));
			__m128 yxz = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 2, 0, 1));
			__m128 tmp0 = _mm_mul_ps(kSiFloat128_2_220, _mm_mul_ps(xxy, yxz));
			
			static const _SiFloat128 kSiFloat128__2_220 = {{{-2.0f, -2.0f, 2.0f, 0.0f}}};
			__m128 wzw = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 3, 2, 3));
			__m128 zzx = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 0, 2, 2));
			__m128 tmp1 = _mm_mul_ps(kSiFloat128__2_220, _mm_mul_ps(wzw, zzx));
			
			// (0,1,0) + tmp0 + tmp1
			tmp0 = _mm_add_ps(tmp0, tmp1);
			m_row[1] = _mm_add_ps(kSiFloat128_0100, tmp0);
		}
		
		//      2*qx*qz+2*qw*qy;      2*qy*qz-2*qw*qx; 1.0f-2*qx*qx-2*qy*qy;
		{
			static const _SiFloat128 kSiFloat128_22_20 = {{{2.0f, 2.0f, -2.0f, 0.0f}}};
			__m128 xyx = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 0, 1, 0));
			__m128 zzx = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 0, 2, 2));
			__m128 tmp0 = _mm_mul_ps(kSiFloat128_22_20, _mm_mul_ps(xyx, zzx));
			
			static const _SiFloat128 kSiFloat128_2_2_20 = {{{2.0f, -2.0f, -2.0f, 0.0f}}};
			__m128 wwy = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 1, 3, 3));
			__m128 yxy = _mm_shuffle_ps(q.Get128(), q.Get128(), _MM_SHUFFLE(0, 1, 0, 1));
			__m128 tmp1 = _mm_mul_ps(kSiFloat128_2_2_20, _mm_mul_ps(wwy, yxy));
			
			// (0,0,1) + tmp0 + tmp1
			tmp0 = _mm_add_ps(tmp0, tmp1);
			m_row[2] = _mm_add_ps(kSiFloat128_0010, tmp0);
		}
	}

	inline Vfloat3x3::Vfloat3x3(
		__m128 row0,
		__m128 row1,
		__m128 row2)
	{
		m_row[0] = row0;
		m_row[1] = row1;
		m_row[2] = row2;
	}

	inline void Vfloat3x3::SetRow(uint32_t rowIndex, Vfloat3_arg row)
	{
		SI_ASSERT(rowIndex<3);
		m_row[rowIndex] = row.Get128();
	}

	inline void Vfloat3x3::SetColumn(uint32_t columnIndex, Vfloat3_arg column)
	{
		SI_ASSERT(columnIndex<3);
		switch(columnIndex)
		{
		case 0:
			m_row[0] = _mm_blend_ps(m_row[0], column.Get128(), 0b0001);
			m_row[1] = _mm_blend_ps(m_row[1], column.Get128(), 0b0001);
			m_row[2] = _mm_blend_ps(m_row[2], column.Get128(), 0b0001);
			break;
		case 1:
			m_row[0] = _mm_blend_ps(m_row[0], column.Get128(), 0b0010);
			m_row[1] = _mm_blend_ps(m_row[1], column.Get128(), 0b0010);
			m_row[2] = _mm_blend_ps(m_row[2], column.Get128(), 0b0010);
			break;
		case 2:
			m_row[0] = _mm_blend_ps(m_row[0], column.Get128(), 0b0100);
			m_row[1] = _mm_blend_ps(m_row[1], column.Get128(), 0b0100);
			m_row[2] = _mm_blend_ps(m_row[2], column.Get128(), 0b0100);
			break;
		default:
			break;
		}
	}

	inline void Vfloat3x3::Set(uint32_t rowIndex, uint32_t columnIndex, Vfloat_arg element)
	{
		SI_ASSERT(rowIndex<3);
		SI_ASSERT(columnIndex<3);
		uint32_t mask = 1<<columnIndex;
		__m128 eeee = _mm_shuffle_ps(element.Get128(), element.Get128(), 0x00);
		m_row[rowIndex] = _si_mm_blend(m_row[rowIndex], eeee, mask);
	}

	inline Vfloat3 Vfloat3x3::GetRow(uint32_t rowIndex) const
	{
		SI_ASSERT(rowIndex<3);
		return Vfloat3(m_row[rowIndex]);
	}

	inline Vfloat3 Vfloat3x3::GetColumn(uint32_t columnIndex) const
	{
		SI_ASSERT(columnIndex<3);
		switch(columnIndex)
		{
		case 0:
			return Vfloat3(
				_mm_unpacklo_ps(
					_mm_shuffle_ps(m_row[0], m_row[2], _MM_SHUFFLE(0,0,0,0)),
					_mm_shuffle_ps(m_row[1], m_row[0], _MM_SHUFFLE(0,0,0,0))));
		case 1:
			return Vfloat3(
				_mm_unpacklo_ps(
					_mm_shuffle_ps(m_row[0], m_row[2], _MM_SHUFFLE(1,1,1,1)),
					_mm_shuffle_ps(m_row[1], m_row[0], _MM_SHUFFLE(1,1,1,1))));
		case 2:
			return Vfloat3(
				_mm_unpacklo_ps(
					_mm_shuffle_ps(m_row[0], m_row[2], _MM_SHUFFLE(2,2,2,2)),
					_mm_shuffle_ps(m_row[1], m_row[0], _MM_SHUFFLE(2,2,2,2))));
		default:
			break;
		}

		return Vfloat3();
	}

	inline Vfloat Vfloat3x3::Get(uint32_t rowIndex, uint32_t columnIndex) const
	{
		SI_ASSERT(rowIndex<3);
		SI_ASSERT(columnIndex<3);
		switch(columnIndex)
		{
		case 0:
			return Vfloat(_mm_shuffle_ps(m_row[rowIndex], m_row[rowIndex], _MM_SHUFFLE(0,0,0,0)));
		case 1:
			return Vfloat(_mm_shuffle_ps(m_row[rowIndex], m_row[rowIndex], _MM_SHUFFLE(1,1,1,1)));
		case 2:
			return Vfloat(_mm_shuffle_ps(m_row[rowIndex], m_row[rowIndex], _MM_SHUFFLE(2,2,2,2)));
		}

		return Vfloat(0.0f);
	}
	
	inline Vfloat3x3 Vfloat3x3::Multiply(Vfloat3x3_arg m) const
	{
		return Math::Multiply(*this, m);
	}

	inline Vfloat3x3 Vfloat3x3::Transpose() const
	{
		return Math::Transpose(*this);
	}
	
	inline Vfloat3 Vfloat3x3::operator[](size_t i) const &&
	{
		SI_ASSERT(i<3);
		return Vfloat3(m_row[i]);
	}
	
	inline Vfloat3x3 Vfloat3x3::operator*(Vfloat3x3_arg m) const
	{
		return Math::Multiply(*this, m);
	}
	
	inline Vfloat4 operator*(Vfloat4_arg v, Vfloat3x3_arg m)
	{
		return Math::Multiply(v, m);
	}
	
	inline __m128 Vfloat3x3::GetRow128(uint32_t rowIndex) const
	{
		SI_ASSERT(rowIndex<3);
		return m_row[rowIndex];
	}
	
	namespace Math
	{
		inline Vfloat3x3 Multiply(Vfloat3x3_arg m0, Vfloat3x3_arg m1)
		{
			__m128 result[3];

			for(uint32_t i=0; i<3; ++i)
			{
				__m128 mi = m0.GetRow128(i);
				__m128 x = _mm_shuffle_ps(mi, mi, _MM_SHUFFLE(0,0,0,0));
				__m128 y = _mm_shuffle_ps(mi, mi, _MM_SHUFFLE(1,1,1,1));
				__m128 z = _mm_shuffle_ps(mi, mi, _MM_SHUFFLE(2,2,2,2));
			
				x = _mm_mul_ps(x, m1.GetRow128(0));
				y = _mm_mul_ps(y, m1.GetRow128(1));
				z = _mm_mul_ps(z, m1.GetRow128(2));

				result[i] = _mm_add_ps(_mm_add_ps(x, y), z);
			}

			return Vfloat3x3(
				result[0],
				result[1],
				result[2]);
		}

		inline Vfloat4 Multiply(Vfloat4_arg v, Vfloat3x3_arg m)
		{
			__m128 vi = v.Get128();
			__m128 x = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(0,0,0,0));
			__m128 y = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(1,1,1,1));
			__m128 z = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(2,2,2,2));
			__m128 w = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(3,3,3,3));
			
			x = _mm_mul_ps(x, m.GetRow128(0));
			y = _mm_mul_ps(y, m.GetRow128(1));
			z = _mm_mul_ps(z, m.GetRow128(2));

			__m128 rot = _mm_add_ps(_mm_add_ps(x, y), z);

			Vfloat4( _mm_blend_ps(rot, w, 0b1000) );
		}

		inline Vfloat3 Multiply(Vfloat3_arg v, Vfloat3x3_arg m)
		{
			__m128 vi = v.Get128();
			__m128 x = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(0,0,0,0));
			__m128 y = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(1,1,1,1));
			__m128 z = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(2,2,2,2));
			
			x = _mm_mul_ps(x, m.GetRow128(0));
			y = _mm_mul_ps(y, m.GetRow128(1));
			z = _mm_mul_ps(z, m.GetRow128(2));

			Vfloat4( _mm_add_ps(_mm_add_ps(x, y), z) );
		}

		inline Vfloat3x3 Transpose(Vfloat3x3_arg m)
		{
			__m128 tmp0 = _mm_shuffle_ps((m.GetRow128(0)), (m.GetRow128(1)),   _MM_SHUFFLE(1,0,1,0));
			__m128 tmp1 = _mm_shuffle_ps((m.GetRow128(0)), (m.GetRow128(1)),   _MM_SHUFFLE(3,2,3,2));
			__m128 tmp2 = _mm_shuffle_ps((m.GetRow128(2)), (kSiFloat128_0000), _MM_SHUFFLE(1,0,1,0));
			__m128 tmp3 = _mm_shuffle_ps((m.GetRow128(2)), (kSiFloat128_0000), _MM_SHUFFLE(3,2,3,2));

			return Vfloat3x3(
				_mm_shuffle_ps(tmp0, tmp2, _MM_SHUFFLE(2,0,2,0)),
				_mm_shuffle_ps(tmp0, tmp2, _MM_SHUFFLE(3,1,3,1)),
				_mm_shuffle_ps(tmp1, tmp3, _MM_SHUFFLE(2,0,2,0)));
		}

		inline Vfloat3x3 Scale3x3(Vfloat3_arg scale)
		{
			return Vfloat3x3(
				_mm_mul_ps( scale.Get128(), kSiFloat128_1000 ),
				_mm_mul_ps( scale.Get128(), kSiFloat128_0100 ),
				_mm_mul_ps( scale.Get128(), kSiFloat128_0010 ));
		}

		inline Vfloat3x3 RotateX3x3(float radian)
		{
			float s = sinf(radian);
			float c = cosf(radian);
			__m128 scsc = _si_mm_set(s,c,s,c);
			__m128 cscs = _mm_shuffle_ps(scsc, scsc, _MM_SHUFFLE(0, 1, 0, 1));
			
			static const _SiFloat128 kSiFloat128_0110  = {{{ 0.0f,  1.0f, 1.0f, 0.0f }}};
			static const _SiFloat128 kSiFloat128_0_110 = {{{ 0.0f, -1.0f, 1.0f, 0.0f }}};

			return Vfloat3x3(
				kSiFloat128_1000,
				_mm_mul_ps( scsc, kSiFloat128_0110),
				_mm_mul_ps( cscs, kSiFloat128_0_110));
		}

		inline Vfloat3x3 RotateY3x3(float radian)
		{
			float s = sinf(radian);
			float c = cosf(radian);
			__m128 sscc = _si_mm_set(s,s,c,c);
			__m128 ccss = _mm_shuffle_ps(sscc, sscc, _MM_SHUFFLE(0, 0, 2, 2));
			
			static const _SiFloat128 kSiFloat128_10_10 = {{{  1.0f, 0.0f,-1.0f, 0.0f }}};
			static const _SiFloat128 kSiFloat128_1010  = {{{  1.0f, 0.0f, 1.0f, 0.0f }}};

			return Vfloat3x3(
				_mm_mul_ps( ccss, kSiFloat128_10_10),
				kSiFloat128_0100,
				_mm_mul_ps( sscc, kSiFloat128_1010));
		}

		inline Vfloat3x3 RotateZ3x3(float radian)
		{
			float s = sinf(radian);
			float c = cosf(radian);
			__m128 scsc = _si_mm_set(s,c,s,c);
			__m128 cscs = _mm_shuffle_ps(scsc, scsc, _MM_SHUFFLE(0, 1, 0, 1));
			
			static const _SiFloat128 kSiFloat128_1100  = {{{  1.0f, 1.0f, 0.0f, 0.0f }}};
			static const _SiFloat128 kSiFloat128__1100 = {{{ -1.0f, 1.0f, 0.0f, 0.0f }}};
			
			return Vfloat3x3(
				_mm_mul_ps( cscs, kSiFloat128_1100),
				_mm_mul_ps( scsc, kSiFloat128__1100),
				kSiFloat128_0010);
		}
	}

} // namespace SI
