#pragma once

#include <xmmintrin.h>
#include <stdint.h>

#include "si_base/math/math_declare.h"

namespace SI
{
	class alignas(16) Vfloat4
	{
	public:
		Vfloat4();		
		Vfloat4(float x, float y, float z, float w);		
		Vfloat4(Vfloat x, Vfloat y, Vfloat z, Vfloat w);
		Vfloat4(Vfloat3 xyz, Vfloat w);
		Vfloat4(Vfloat3 xyz, float w);
		explicit Vfloat4(float value);		
		explicit Vfloat4(Vfloat_arg value);		
		explicit Vfloat4(const float* v);
		explicit Vfloat4(__m128 v);
		
	public:
		void SetX(Vfloat_arg x);
		void SetY(Vfloat_arg y);		
		void SetZ(Vfloat_arg z);
		void SetW(Vfloat_arg w);
		void SetElement(uint32_t elementIndex, Vfloat_arg e);
		void Set(float value);
		void Set(Vfloat_arg value);		
		void Set(const float* v);
		void Set(float x, float y, float z, float w);
		void Set(Vfloat_arg x, Vfloat_arg y, Vfloat_arg z, Vfloat_arg w);
		
	public:
		Vfloat X() const;
		Vfloat Y() const;
		Vfloat Z() const;
		Vfloat W() const;
		float  Xf() const;
		float  Yf() const;
		float  Zf() const;
		float  Wf() const;
		Vfloat GetElement(uint32_t elementIndex) const;

		// 要素を入れ替えるSwizzle関数. templateの方はSIMD使えるので早い.
		template<uint32_t xIndex, uint32_t yIndex, uint32_t zIndex, uint32_t wIndex>
		Vfloat4 Swizzle() const;
		Vfloat4 Swizzle(uint32_t xIndex, uint32_t yIndex, uint32_t zIndex, uint32_t wIndex) const;
		
		Vfloat3 XYZ() const;
		Vfloat4 XYZ0() const;
		Vfloat4 XYZ1() const;

	public:
		Vfloat4 Multiply(Vfloat4x4_arg m) const;
		Vfloat  LengthSqr() const;
		Vfloat  Length() const;
		Vfloat4 Normalize() const;
		Vfloat4 NormalizeFast() const;

	public:
		Vfloat4 operator-() const;
		Vfloat operator[](size_t i) const &&; // [] operatorは代入を許可しないようにしておく.
		Vfloat4 operator+(Vfloat4_arg v) const;
		Vfloat4 operator-(Vfloat4_arg v) const;
		Vfloat4 operator*(Vfloat4_arg v) const;
		Vfloat4 operator/(Vfloat4_arg v) const;
		Vfloat4 operator*(Vfloat_arg f) const;
		Vfloat4 operator/(Vfloat_arg f) const;
		Vfloat4 operator*(float f) const;
		Vfloat4 operator/(float f) const;
		Vfloat4 operator*(Vfloat4x4_arg m) const;
		Vfloat4 operator*(Vfloat4x3_arg m) const;
		Vfloat4& operator+=(Vfloat4_arg v);
		Vfloat4& operator-=(Vfloat4_arg v);

	public:
		static Vfloat4 Zero();
		static Vfloat4 One();
		static Vfloat4 AxisX();
		static Vfloat4 AxisY();
		static Vfloat4 AxisZ();
		static Vfloat4 AxisW();

	public:
		__m128 Get128() const;

	private:
		__m128 m_v;
	};
	
	Vfloat4 operator*(Vfloat f, Vfloat4_arg v);
	Vfloat4 operator/(Vfloat f, Vfloat4_arg v);
	Vfloat4 operator*(float  f, Vfloat4_arg v);
	Vfloat4 operator/(float  f, Vfloat4_arg v);

	namespace Math
	{
		Vfloat4 Min       (Vfloat4_arg a, Vfloat4_arg b);
		Vfloat4 Max       (Vfloat4_arg a, Vfloat4_arg b);
		Vfloat4 Sqrt      (Vfloat4_arg a);
		Vfloat4 Rsqrt     (Vfloat4_arg a);
		Vfloat4 Rcp       (Vfloat4_arg a);
		Vfloat  LengthSqr (Vfloat4_arg a);
		Vfloat  Length    (Vfloat4_arg a);
		Vfloat4 Normalize (Vfloat4_arg a);
		Vfloat4 NormalizeFast (Vfloat4_arg a);

	} // namespace Math
}

#include "si_base/math/inl/vfloat4.inl"
