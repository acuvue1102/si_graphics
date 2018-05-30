#pragma once

#include "si_base/math/Vfloat4x3.h"

#include <stdint.h>
#include <xmmintrin.h>
#include <smmintrin.h>
#include "si_base/core/assert.h"
#include "si_base/math/vfloat.h"
#include "si_base/math/vfloat4.h"

namespace SI
{
	inline Vfloat4x3::Vfloat4x3()
	{
		m_row[0] = kSiFloat128_1000;
		m_row[1] = kSiFloat128_0100;
		m_row[2] = kSiFloat128_0010;
		m_row[3] = kSiFloat128_0000;
	}

	inline Vfloat4x3::Vfloat4x3(
		Vfloat3_arg row0,
		Vfloat3_arg row1,
		Vfloat3_arg row2,
		Vfloat3_arg row3)
	{
		m_row[0] = row0.Get128();
		m_row[1] = row1.Get128();
		m_row[2] = row2.Get128();
		m_row[3] = row3.Get128();
	}

	inline Vfloat4x3::Vfloat4x3(
		Vfloat3x3_arg rot,
		Vfloat3_arg trans)
	{
		m_row[0] = rot.GetRow128(0);
		m_row[1] = rot.GetRow128(1);
		m_row[2] = rot.GetRow128(2);
		m_row[3] = trans.Get128();
	}

	inline Vfloat4x3::Vfloat4x3(
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

	inline void Vfloat4x3::SetRow(uint32_t rowIndex, Vfloat3_arg row)
	{
		SI_ASSERT(rowIndex<4);
		m_row[rowIndex] = row.Get128();
	}

	inline void Vfloat4x3::SetColumn(uint32_t columnIndex, Vfloat4_arg column)
	{
		SI_ASSERT(columnIndex<3);
		__m128 mask = _si_mm_get_mask(1<<columnIndex);
		m_row[0] = _si_mm_blend(m_row[0], column.Get128(), mask);
		m_row[1] = _si_mm_blend(m_row[1], column.Get128(), mask);
		m_row[2] = _si_mm_blend(m_row[2], column.Get128(), mask);
		m_row[3] = _si_mm_blend(m_row[3], column.Get128(), mask);
	}

	inline void Vfloat4x3::Set(uint32_t rowIndex, uint32_t columnIndex, Vfloat_arg element)
	{
		SI_ASSERT(rowIndex<4);
		SI_ASSERT(columnIndex<3);

		uint32_t mask = 1<<columnIndex;
		__m128 eeee = _mm_shuffle_ps(element.Get128(), element.Get128(), 0x00);
		m_row[rowIndex] = _si_mm_blend(m_row[rowIndex], eeee, mask);
	}
	
	inline void Vfloat4x3::SetUpper3x3(Vfloat3x3_arg m)
	{
		m_row[0] = m.GetRow128(0);
		m_row[1] = m.GetRow128(1);
		m_row[2] = m.GetRow128(2);
	}

	inline Vfloat3 Vfloat4x3::GetRow(uint32_t rowIndex) const
	{
		SI_ASSERT(rowIndex<4);
		return Vfloat3(m_row[rowIndex]);
	}

	inline Vfloat4 Vfloat4x3::GetColumn(uint32_t columnIndex) const
	{
		SI_ASSERT(columnIndex<3);
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
		default:
			break;
		}

		return Vfloat4();
	}

	inline Vfloat Vfloat4x3::Get(uint32_t rowIndex, uint32_t columnIndex) const
	{
		SI_ASSERT(rowIndex<4);
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

	inline Vfloat3x3 Vfloat4x3::GetUpper3x3() const
	{
		return Vfloat3x3(
			m_row[0],
			m_row[1],
			m_row[2]);
	}
	
	inline Vfloat4x3 Vfloat4x3::Multiply(Vfloat4x3_arg m) const
	{
		return Math::Multiply(*this, m);
	}
	
	inline Vfloat3 Vfloat4x3::operator[](size_t i) const &&
	{
		return Vfloat3(m_row[i]);
	}
	
	inline Vfloat4x3 Vfloat4x3::operator*(Vfloat4x3_arg m) const
	{
		return Math::Multiply(*this, m);
	}
	
	inline Vfloat4x3 Vfloat4x3::Translate(Vfloat3_arg translate)
	{
		return Math::Translate4x3(translate);
	}

	inline Vfloat4x3 Vfloat4x3::Scale(Vfloat3_arg scale)
	{
		return Math::Scale4x3(scale);
	}

	inline Vfloat4x3 Vfloat4x3::RotateX(float radian)
	{
		return Math::RotateX4x3(radian);
	}

	inline Vfloat4x3 Vfloat4x3::RotateY(float radian)
	{
		return Math::RotateY4x3(radian);
	}

	inline Vfloat4x3 Vfloat4x3::RotateZ(float radian)
	{
		return Math::RotateZ4x3(radian);
	}

	inline Vfloat4 operator*(Vfloat4_arg v, Vfloat4x3_arg m)
	{
		return Math::Multiply(v, m);
	}
	
	inline __m128 Vfloat4x3::GetRow128(uint32_t rowIndex) const
	{
		SI_ASSERT(rowIndex<4);
		return m_row[rowIndex];
	}
	
	namespace Math
	{
		inline Vfloat4x3 Multiply(Vfloat4x3_arg m0, Vfloat4x3_arg m1)
		{
			__m128 result[4];

			for(uint32_t i=0; i<4; ++i)
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
			
			result[3] = _mm_add_ps(result[3], m1.GetRow128(3));

			return Vfloat4x3(
				result[0],
				result[1],
				result[2],
				result[3]);
		}

		inline Vfloat4 Multiply(Vfloat4_arg v, Vfloat4x3_arg m)
		{
			__m128 vi = v.Get128();
			__m128 x = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(0,0,0,0));
			__m128 y = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(1,1,1,1));
			__m128 z = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(2,2,2,2));
			__m128 w = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(3,3,3,3));
			
			        x = _mm_mul_ps(x, m.GetRow128(0));
			        y = _mm_mul_ps(y, m.GetRow128(1));
			        z = _mm_mul_ps(z, m.GetRow128(2));
			__m128 w2 = _mm_mul_ps(w, m.GetRow128(3)); // wは後で使うので別変数に入れる.

			__m128 rot = _mm_add_ps(_mm_add_ps(x, y), _mm_add_ps(z, w2));
			
			Vfloat4(_mm_blend_ps(rot, w, 0b1000)); // rot.xyz, w
		}

		inline Vfloat3 Multiply(Vfloat3_arg v, Vfloat4x3_arg m)
		{
			__m128 vi = v.Get128();
			__m128 x = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(0,0,0,0));
			__m128 y = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(1,1,1,1));
			__m128 z = _mm_shuffle_ps(vi, vi, _MM_SHUFFLE(2,2,2,2));
			
			x = _mm_mul_ps(x, m.GetRow128(0));
			y = _mm_mul_ps(y, m.GetRow128(1));
			z = _mm_mul_ps(z, m.GetRow128(2));

			return Vfloat3( _mm_add_ps(_mm_add_ps(x, y), _mm_add_ps(z, m.GetRow128(3))) );
		}
		
		inline Vfloat4x3 LookAtMatrix(Vfloat3_arg pos, Vfloat3_arg target, Vfloat3_arg up)
		{
			return LookToMatrix(pos, target - pos, up);
		}
		
		inline Vfloat4x3 LookToMatrix(Vfloat3_arg pos, Vfloat3_arg targetDir, Vfloat3_arg up)
		{
			Vfloat3 newZ = Normalize(targetDir);
			Vfloat3 newX = Normalize(Cross(up, newZ));
			Vfloat3 newY = Cross(newZ, newX);

			__m128 minusPos = (-pos).Get128();
			__m128 dX = _mm_dp_ps(newX.Get128(), minusPos, 0xff);
			__m128 dY = _mm_dp_ps(newY.Get128(), minusPos, 0xff);
			__m128 dZ = _mm_dp_ps(newZ.Get128(), minusPos, 0xff);
			
			dX = _mm_blend_ps(newX.Get128(), dX, 0b1000);
			dY = _mm_blend_ps(newY.Get128(), dY, 0b1000);
			dZ = _mm_blend_ps(newZ.Get128(), dZ, 0b1000);
			__m128 dW = kSiFloat128_0001;
			
			// transpose.
			__m128 tmp0 = _mm_shuffle_ps(dX, dY, _MM_SHUFFLE(1,0,1,0));
			__m128 tmp1 = _mm_shuffle_ps(dX, dY, _MM_SHUFFLE(3,2,3,2));
			__m128 tmp2 = _mm_shuffle_ps(dZ, dW, _MM_SHUFFLE(1,0,1,0));
			__m128 tmp3 = _mm_shuffle_ps(dZ, dW, _MM_SHUFFLE(3,2,3,2));

			return Vfloat4x3(
				_mm_shuffle_ps(tmp0, tmp2, _MM_SHUFFLE(2,0,2,0)),
				_mm_shuffle_ps(tmp0, tmp2, _MM_SHUFFLE(3,1,3,1)),
				_mm_shuffle_ps(tmp1, tmp3, _MM_SHUFFLE(2,0,2,0)),
				_mm_shuffle_ps(tmp1, tmp3, _MM_SHUFFLE(3,1,3,1)));
		}
		
		inline Vfloat4x3 Translate4x3(Vfloat3_arg translate)
		{
			return Vfloat4x3(
				kSiFloat128_1000,
				kSiFloat128_0100,
				kSiFloat128_0010,
				_mm_blend_ps(translate.Get128(), kSiFloat128_1111, 0b1000) );
		}

		inline Vfloat4x3 Scale4x3(Vfloat3_arg scale)
		{
			return Vfloat4x3(
				_mm_mul_ps( scale.Get128(), kSiFloat128_1000 ),
				_mm_mul_ps( scale.Get128(), kSiFloat128_0100 ),
				_mm_mul_ps( scale.Get128(), kSiFloat128_0010 ),
				kSiFloat128_0001);
		}

		inline Vfloat4x3 RotateX4x3(float radian)
		{
			float s = sinf(radian);
			float c = cosf(radian);
			__m128 scsc = _si_mm_set(s,c,s,c);
			__m128 cscs = _mm_shuffle_ps(scsc, scsc, _MM_SHUFFLE(0, 1, 0, 1));
			
			static const _SiFloat128 kSiFloat128_0110  = {{{ 0.0f,  1.0f, 1.0f, 0.0f }}};
			static const _SiFloat128 kSiFloat128_0_110 = {{{ 0.0f, -1.0f, 1.0f, 0.0f }}};

			return Vfloat4x3(
				kSiFloat128_1000,
				_mm_mul_ps( scsc, kSiFloat128_0110),
				_mm_mul_ps( cscs, kSiFloat128_0_110),
				kSiFloat128_0001);
		}

		inline Vfloat4x3 RotateY4x3(float radian)
		{
			float s = sinf(radian);
			float c = cosf(radian);
			__m128 sscc = _si_mm_set(s,s,c,c);
			__m128 ccss = _mm_shuffle_ps(sscc, sscc, _MM_SHUFFLE(0, 0, 2, 2));
			
			static const _SiFloat128 kSiFloat128_10_10 = {{{  1.0f, 0.0f,-1.0f, 0.0f }}};
			static const _SiFloat128 kSiFloat128_1010  = {{{  1.0f, 0.0f, 1.0f, 0.0f }}};

			return Vfloat4x3(
				_mm_mul_ps( ccss, kSiFloat128_10_10),
				kSiFloat128_0100,
				_mm_mul_ps( sscc, kSiFloat128_1010),
				kSiFloat128_0001);
		}

		inline Vfloat4x3 RotateZ4x3(float radian)
		{
			float s = sinf(radian);
			float c = cosf(radian);
			__m128 scsc = _si_mm_set(s,c,s,c);
			__m128 cscs = _mm_shuffle_ps(scsc, scsc, _MM_SHUFFLE(0, 1, 0, 1));
			
			static const _SiFloat128 kSiFloat128_1100  = {{{  1.0f, 1.0f, 0.0f, 0.0f }}};
			static const _SiFloat128 kSiFloat128__1100 = {{{ -1.0f, 1.0f, 0.0f, 0.0f }}};
			
			return Vfloat4x3(
				_mm_mul_ps( cscs, kSiFloat128_1100),
				_mm_mul_ps( scsc, kSiFloat128__1100),
				kSiFloat128_0010,
				kSiFloat128_0001);
		}
	}

} // namespace SI
