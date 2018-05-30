#pragma once

#include <stdint.h>
#include <xmmintrin.h>

#include "si_base/math/math_declare.h"

namespace SI
{
	class Vfloat4x3
	{
	public:
		Vfloat4x3();

		Vfloat4x3(
			Vfloat3_arg row0,
			Vfloat3_arg row1,
			Vfloat3_arg row2,
			Vfloat3_arg row3);

		Vfloat4x3(
			Vfloat3x3_arg rot,
			Vfloat3_arg trans);

		Vfloat4x3(
			__m128 row0,
			__m128 row1,
			__m128 row2,
			__m128 row3);

	public:
		void SetRow   (uint32_t rowIndex,    Vfloat3_arg row);
		void SetColumn(uint32_t columnIndex, Vfloat4_arg column);

		void Set(
			uint32_t   rowIndex,
			uint32_t   columnIndex,
			Vfloat_arg element);

		void SetUpper3x3(Vfloat3x3_arg m);
		
	public:
		Vfloat3 GetRow   (uint32_t rowIndex) const;
		Vfloat4 GetColumn(uint32_t columnIndex) const;

		Vfloat Get(
			uint32_t rowIndex,
			uint32_t columnIndex) const;

		Vfloat3x3 GetUpper3x3() const;

	public:
		Vfloat4x3 Multiply(Vfloat4x3_arg m) const;

	public:
		Vfloat3   operator[](size_t i) const &&; // [] operatorは代入を許可しないようにしておく.
		Vfloat4x3 operator*(Vfloat4x3_arg m) const;

	public:
		static Vfloat4x3 Translate(Vfloat3_arg);
		static Vfloat4x3 Scale(Vfloat3_arg);
		static Vfloat4x3 RotateX(float radian);
		static Vfloat4x3 RotateY(float radian);
		static Vfloat4x3 RotateZ(float radian);
				
	public:
		__m128 GetRow128(uint32_t rowIndex) const;

	private:
		__m128   m_row[4];
	};

	Vfloat4 operator*(Vfloat4_arg v, Vfloat4x3_arg m);
	
	namespace Math
	{
		Vfloat4x3 Multiply(Vfloat4x3_arg m0, Vfloat4x3_arg m1);
		Vfloat4   Multiply(Vfloat4_arg    v, Vfloat4x3_arg m);
		Vfloat3   Multiply(Vfloat3_arg    v, Vfloat4x3_arg m);
		
		Vfloat4x3 LookAtMatrix(Vfloat3_arg pos, Vfloat3_arg target, Vfloat3_arg up);
		Vfloat4x3 LookToMatrix(Vfloat3_arg pos, Vfloat3_arg targetDir, Vfloat3_arg up);
		Vfloat4x3 Translate4x3(Vfloat3_arg);
		Vfloat4x3 Scale4x3(Vfloat3_arg);
		Vfloat4x3 RotateX4x3(float radian);
		Vfloat4x3 RotateY4x3(float radian);
		Vfloat4x3 RotateZ4x3(float radian);
	}

} // namespace SI

#include "si_base/math/inl/vfloat4x3.inl"
