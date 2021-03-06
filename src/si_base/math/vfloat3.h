﻿#pragma once

#include <xmmintrin.h>
#include <cstdint>

#include "si_base/math/math_declare.h"

namespace SI
{
	struct PackedFloat3
	{
		float m_v[3];
	};

	class alignas(16) Vfloat3
	{
	public:
		Vfloat3();
		Vfloat3(const Vfloat3& v);
		Vfloat3(float x, float y, float z);
		Vfloat3(Vfloat x, Vfloat y, Vfloat z);		
		explicit Vfloat3(float value);		
		explicit Vfloat3(Vfloat_arg value);		
		explicit Vfloat3(const float* v);
		explicit Vfloat3(__m128 v);
		
	public:
		void SetX(Vfloat_arg x);
		void SetY(Vfloat_arg y);		
		void SetZ(Vfloat_arg z);
		void SetElement(uint32_t elementIndex, Vfloat_arg e);
		void Set(float value);
		void Set(Vfloat_arg value);		
		void Set(const float* v);
		void Set(float x, float y, float z);
		void Set(Vfloat_arg x, Vfloat_arg y, Vfloat_arg z);
		
	public:
		Vfloat X() const;
		Vfloat Y() const;
		Vfloat Z() const;
		float  Xf() const;
		float  Yf() const;
		float  Zf() const;
		Vfloat GetElement(uint32_t elementIndex) const;
		
		// 要素を入れ替えるSwizzle関数. templateの方はSIMD使えるので早い.
		template<uint32_t xIndex, uint32_t yIndex, uint32_t zIndex>
		Vfloat3 Swizzle() const;
		Vfloat3 Swizzle(uint32_t xIndex, uint32_t yIndex, uint32_t zIndex) const;		
		
		Vfloat4 XYZ0() const;
		Vfloat4 XYZ1() const;

	public:
		Vfloat  LengthSqr() const;
		Vfloat  Length() const;
		Vfloat3 Normalize() const;
		Vfloat3 NormalizeFast() const;

	public:
		PackedFloat3 GetPackedFloat3() const;

	public:
		Vfloat3& operator=(const Vfloat3& v);
		Vfloat3  operator-() const;
		const Vfloat   operator[](size_t i) const; // [] operatorは代入を許可しないようにしておく.
		Vfloat3  operator+(Vfloat3_arg v) const;
		Vfloat3  operator-(Vfloat3_arg v) const;
		Vfloat3  operator*(Vfloat3_arg v) const;
		Vfloat3  operator/(Vfloat3_arg v) const;
		Vfloat3  operator*(Vfloat_arg f) const;
		Vfloat3  operator/(Vfloat_arg f) const;
		Vfloat3  operator*(float f) const;
		Vfloat3  operator/(float f) const;
		Vfloat3  operator*(Vfloat4x3_arg m) const;
		Vfloat3  operator*(Vquat_arg q) const;
		Vfloat3& operator+=(Vfloat3_arg v);
		Vfloat3& operator-=(Vfloat3_arg v);
		Vfloat3& operator*=(Vfloat3_arg v);
		Vfloat3& operator/=(Vfloat3_arg v);
		Vfloat3& operator*=(float f);
		Vfloat3& operator/=(float f);
		bool     operator==(const Vfloat3& v) const;
		bool     operator!=(const Vfloat3& v) const;

	public:
		static Vfloat3 Zero();
		static Vfloat3 One();
		static Vfloat3 AxisX();
		static Vfloat3 AxisY();
		static Vfloat3 AxisZ();

	public:
		__m128 Get128() const;

	private:
		__m128 m_v;
	};
	
	Vfloat3 operator*(Vfloat f, Vfloat3_arg v);
	Vfloat3 operator/(Vfloat f, Vfloat3_arg v);
	Vfloat3 operator*(float f,  Vfloat3_arg v);
	Vfloat3 operator/(float f,  Vfloat3_arg v);

	namespace Math
	{
		Vfloat3 Min           (Vfloat3_arg a, Vfloat3_arg b);
		Vfloat3 Max           (Vfloat3_arg a, Vfloat3_arg b);
		Vfloat  HorizontalMin (Vfloat3_arg a);
		Vfloat  HorizontalMax (Vfloat3_arg a);
		Vfloat  HorizontalAdd (Vfloat3_arg a);
		Vfloat  HorizontalMul (Vfloat3_arg a);
		Vfloat3 Abs           (Vfloat3_arg a);
		Vfloat3 Sqrt          (Vfloat3_arg a);
		Vfloat3 Rsqrt         (Vfloat3_arg a);
		Vfloat3 Rcp           (Vfloat3_arg a);
		Vfloat  Dot           (Vfloat3_arg a, Vfloat3_arg b);
		Vfloat3 Cross         (Vfloat3_arg a, Vfloat3_arg b);
		Vfloat  LengthSqr     (Vfloat3_arg a);
		Vfloat  Length        (Vfloat3_arg a);
		Vfloat3 Normalize     (Vfloat3_arg a);
		Vfloat3 NormalizeFast (Vfloat3_arg a);
		Vfloat3 Floor         (Vfloat3_arg a);

	} // namespace Math
}

#include "si_base/math/inl/vfloat3.inl"
