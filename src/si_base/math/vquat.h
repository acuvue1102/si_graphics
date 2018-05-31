#pragma once

#include <xmmintrin.h>
#include <cstdint>

#include "si_base/math/math_declare.h"

namespace SI
{
	class alignas(16) Vquat
	{
	public:
		Vquat();		
		Vquat(float x, float y, float z, float w);
		Vquat(Vfloat x, Vfloat y, Vfloat z, Vfloat w);
		Vquat(Vfloat4 xyzw);
		Vquat(Vfloat3 axis, float angle);
		Vquat(Vfloat3 axis, Vfloat angle);
		explicit Vquat(const float* v);
		explicit Vquat(__m128 v);
		
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
		Vfloat GetElement(uint32_t elementIndex) const;
		
	public:
		Vquat Normalize() const;
		Vquat Conjugate() const;
		Vquat Inverse() const;

	public:
		Vfloat operator[](size_t i) const &&; // [] operatorは代入を許可しないようにしておく.
		Vquat operator-() const;
		Vquat operator*(Vquat_arg q);
		Vquat& operator*=(Vquat_arg q);

	public:
		__m128 Get128() const;

	private:
		__m128 m_v;
	};

	namespace Math
	{
		Vquat Normalize(Vquat_arg q);
		Vquat Conjugate(Vquat_arg q);
		Vquat Inverse(Vquat_arg q);

		// 軸v周りのラジアンr回転
		Vquat CreateQuat(Vfloat3_arg v, Vfloat_arg r);
		Vquat CreateQuat(Vfloat3_arg v, float r);
		
		Vquat   Multiply(Vquat_arg   a, Vquat_arg b);
		Vfloat3 Multiply(Vfloat3_arg v, Vquat_arg q);

	} // namespace Math
}

#include "si_base/math/inl/Vquat.inl"
