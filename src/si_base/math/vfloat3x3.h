#pragma once

#include <cstdint>
#include <xmmintrin.h>

#include "si_base/math/math_declare.h"

namespace SI
{
	class Vfloat3x3
	{
	public:
		Vfloat3x3();

		Vfloat3x3(
			Vfloat3_arg row0,
			Vfloat3_arg row1,
			Vfloat3_arg row2);

		Vfloat3x3(
			Vquat_arg q);

		Vfloat3x3(
			__m128 row0,
			__m128 row1,
			__m128 row2);

	public:
		void SetRow   (uint32_t rowIndex,    Vfloat3_arg row);
		void SetColumn(uint32_t columnIndex, Vfloat3_arg column);

		void Set(
			uint32_t   rowIndex,
			uint32_t   columnIndex,
			Vfloat_arg element);
		
	public:
		Vfloat3 GetRow   (uint32_t rowIndex) const;
		Vfloat3 GetColumn(uint32_t columnIndex) const;

		Vfloat Get(
			uint32_t rowIndex,
			uint32_t columnIndex) const;

	public:
		Vfloat3x3 Multiply(Vfloat3x3_arg m) const;
		Vfloat3x3 Transpose() const;

	public:
		Vfloat3   operator[](size_t i) const; // [] operatorは代入を許可しないようにしておく.
		Vfloat3x3 operator*(Vfloat3x3_arg m) const;
				
	public:
		__m128 GetRow128(uint32_t rowIndex) const;

	private:
		__m128   m_row[3];
	};

	Vfloat4 operator*(Vfloat4_arg v, Vfloat3x3_arg m);
	
	namespace Math
	{
		Vfloat3x3 Multiply(Vfloat3x3_arg m0, Vfloat3x3_arg m1);
		Vfloat4   Multiply(Vfloat4_arg    v, Vfloat3x3_arg m);
		Vfloat3   Multiply(Vfloat3_arg    v, Vfloat3x3_arg m);
		Vfloat3x3 Transpose(Vfloat3x3_arg m);
		Vfloat3x3 Scale3x3(Vfloat3_arg);
		Vfloat3x3 RotateX3x3(float radian);
		Vfloat3x3 RotateY3x3(float radian);
		Vfloat3x3 RotateZ3x3(float radian);
	}

} // namespace SI

#include "si_base/math/inl/Vfloat3x3.inl"
