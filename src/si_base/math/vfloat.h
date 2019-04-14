#pragma once

#include <xmmintrin.h>

namespace SI
{
	class alignas(16) Vfloat
	{
	public:
		Vfloat();
		Vfloat(const Vfloat& v);
		Vfloat(float value); // no explicit
		explicit Vfloat(const __m128& v);
		
	public:
		void Set(float v);

	public:
		float Get() const;		
		float AsFloat() const;

	public:
		Vfloat& operator=(const Vfloat& v);
		operator float() const;
		Vfloat operator+(Vfloat_arg r) const;
		Vfloat operator-(Vfloat_arg r) const;
		Vfloat operator*(Vfloat_arg r) const;
		Vfloat operator/(Vfloat_arg r) const;

	public:
		__m128 Get128() const;

	private:
		__m128 m_v;
	};
	
	Vfloat operator+(float f, Vfloat_arg vf);	
	Vfloat operator-(float f, Vfloat_arg vf);	
	Vfloat operator*(float f, Vfloat_arg vf);	
	Vfloat operator/(float f, Vfloat_arg vf);
	Vfloat operator+(Vfloat_arg vf, float f);	
	Vfloat operator-(Vfloat_arg vf, float f);	
	Vfloat operator*(Vfloat_arg vf, float f);	
	Vfloat operator/(Vfloat_arg vf, float f);

	namespace Math
	{
		Vfloat Min  (const Vfloat& a, const Vfloat& b);
		Vfloat Max  (const Vfloat& a, const Vfloat& b);
		Vfloat Abs  (const Vfloat& a);
		Vfloat Sqrt (const Vfloat& a);
		Vfloat Rsqrt(const Vfloat& a);
		Vfloat Rcp  (const Vfloat& a);
		Vfloat Floor(const Vfloat& a);

	} // namespace Math
}

#include "si_base/math/inl/vfloat.inl"
