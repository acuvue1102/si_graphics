#pragma once

#include <cstdint>
#include <xmmintrin.h>

#include "si_base/math/math_declare.h"
namespace SI
{
	class Vfloat4x4
	{
	public:
		Vfloat4x4();

		Vfloat4x4(const Vfloat4x4& m);

		Vfloat4x4(
			Vfloat4_arg row0,
			Vfloat4_arg row1,
			Vfloat4_arg row2,
			Vfloat4_arg row3);
		
		Vfloat4x4(
			Vfloat4x3_arg m);
		
		Vfloat4x4(
			Vfloat3x3_arg rot,
			Vfloat3_arg trans);

		Vfloat4x4(
			__m128 row0,
			__m128 row1,
			__m128 row2,
			__m128 row3);

	public:
		void SetRow   (uint32_t rowIndex,    Vfloat4_arg row);
		void SetColumn(uint32_t columnIndex, Vfloat4_arg column);

		void Set(
			uint32_t   rowIndex,
			uint32_t   columnIndex,
			Vfloat_arg element);

		void SetUpper3x3(Vfloat3x3_arg m);
		
	public:
		Vfloat4 GetRow   (uint32_t rowIndex) const;
		Vfloat4 GetColumn(uint32_t columnIndex) const;

		Vfloat Get(
			uint32_t rowIndex,
			uint32_t columnIndex) const;
		
		Vfloat3x3 GetUpper3x3() const;

	public:
		Vfloat4x4 Multiply(Vfloat4x4_arg m) const;
		Vfloat4x4 Transpose() const;

	public:
		Vfloat4x4& operator=(const Vfloat4x4& m);
		Vfloat4   operator[](size_t i) const; // [] operatorは代入を許可しないようにしておく.
		Vfloat4x4 operator*(Vfloat4x4_arg m) const;

	public:
		static Vfloat4x4 Translate(Vfloat3_arg);
		static Vfloat4x4 Scale(Vfloat3_arg);
		static Vfloat4x4 RotateX(float radian);
		static Vfloat4x4 RotateY(float radian);
		static Vfloat4x4 RotateZ(float radian);
		static Vfloat4x4 Identity();
				
	public:
		__m128 GetRow128(uint32_t rowIndex) const;

	private:
		__m128   m_row[4];
	};
	
	Vfloat4   operator*(Vfloat4_arg v,    Vfloat4x4_arg m);
	Vfloat4x4 operator*(Vfloat4x4_arg m0, Vfloat4x3_arg m1);
	Vfloat4x4 operator*(Vfloat4x3_arg m0, Vfloat4x4_arg m1);
	
	namespace Math
	{
		Vfloat4x4 Multiply(Vfloat4x4_arg m0, Vfloat4x4_arg m1);
		Vfloat4x4 Multiply(Vfloat4x4_arg m0, Vfloat4x3_arg m1);
		Vfloat4x4 Multiply(Vfloat4x3_arg m0, Vfloat4x4_arg m1);
		Vfloat4   Multiply(Vfloat4_arg    v, Vfloat4x4_arg m);
		Vfloat3   Multiply(Vfloat3_arg    v, Vfloat4x4_arg m);
		Vfloat4x4 Transpose(Vfloat4x4_arg m);
		Vfloat4x4 Perspective(float width, float height, float near, float far);
		Vfloat4x4 Ortho(float width, float height, float near, float far);
		Vfloat4x4 Translate4x4(Vfloat3_arg);
		Vfloat4x4 Scale4x4(Vfloat3_arg);
		Vfloat4x4 RotateX4x4(float radian);
		Vfloat4x4 RotateY4x4(float radian);
		Vfloat4x4 RotateZ4x4(float radian);
	}

} // namespace SI

#include "si_base/math/inl/vfloat4x4.inl"
