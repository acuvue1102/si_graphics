#pragma once

#include <xmmintrin.h>
#include <cstdint>

#include "si_base/core/constant.h"
#include "si_base/math/vfloat3.h"

namespace SI
{
	class Aabb
	{
	public:
		Aabb()
			: m_min(-0.5f)
			, m_max(0.5f)
		{
		}

		~Aabb(){}


		void SetMinMax(Vfloat3_arg aabbMin, Vfloat3_arg aabbMax)
		{
			SI_ASSERT(aabbMin.Xf() < aabbMax.Xf());
			SI_ASSERT(aabbMin.Yf() < aabbMax.Yf());
			SI_ASSERT(aabbMin.Zf() < aabbMax.Zf());

			m_min = aabbMin;
			m_max = aabbMax;
		}

		void SetCenterExtend(Vfloat3_arg aabbCenter, Vfloat3_arg aabbExtend)
		{
			SI_ASSERT(0.0f < aabbExtend.Xf());
			SI_ASSERT(0.0f < aabbExtend.Yf());
			SI_ASSERT(0.0f < aabbExtend.Zf());

			m_min = aabbCenter - aabbExtend;
			m_max = aabbCenter + aabbExtend;
		}
		
		Vfloat3 GetMin()    const{ return m_min; }
		Vfloat3 GetMax()    const{ return m_max; }
		Vfloat3 GetCenter() const{ return Vfloat(0.5f) * (m_min + m_max); }
		Vfloat3 GetExtend() const{ return Vfloat(0.5f) * (m_max - m_min); }

	private:
		Vfloat3 m_min;
		Vfloat3 m_max;
	};
}
