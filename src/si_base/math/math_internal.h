#pragma once

// 内部で使用するための定義など.
// このファイルは基本的にユーザが見る必要はない.

#include <cstdint>
#include <xmmintrin.h>
#include "si_base/core/basic_macro.h"

namespace SI
{
	using __si128 = __m128;

	struct alignas(16) _SiFloat128
	{
		union
		{
			float    m_f[4];
			__si128  m_v;
		};

		inline operator __si128() const { return m_v; }
		inline operator const float*() const { return m_f; }
	};

	struct alignas(16) _SiUint128
	{
		union
		{
			uint32_t  m_u[4];
			__si128   m_v;
		};

		inline operator __si128() const { return m_v; }
		inline operator const uint32_t*() const { return m_u; }
	};
	
	SI_GLOBAL_CONST _SiFloat128 kSiFloat128_0000 = {{{ 0.0f, 0.0f, 0.0f, 0.0f }}};
	SI_GLOBAL_CONST _SiFloat128 kSiFloat128_1111 = {{{ 1.0f, 1.0f, 1.0f, 1.0f }}};
	
	SI_GLOBAL_CONST _SiFloat128 kSiFloat128_1000 = {{{ 1.0f, 0.0f, 0.0f, 0.0f }}};
	SI_GLOBAL_CONST _SiFloat128 kSiFloat128_0100 = {{{ 0.0f, 1.0f, 0.0f, 0.0f }}};
	SI_GLOBAL_CONST _SiFloat128 kSiFloat128_0010 = {{{ 0.0f, 0.0f, 1.0f, 0.0f }}};
	SI_GLOBAL_CONST _SiFloat128 kSiFloat128_0001 = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
	
	SI_GLOBAL_CONST _SiUint128  kSiUint128_AbsMask = {{{ 0x8fffffff, 0x8fffffff, 0x8fffffff, 0x8fffffff }}};

	inline __m128 _si_mm_get_mask(int ctrl)
	{
		static const _SiUint128 kMaskTable[] =
		{
			{{{ 0u        , 0u,         0u,         0u         }}},
			{{{ UINT32_MAX, 0u,         0u,         0u         }}},
			{{{ 0u,         UINT32_MAX, 0u,         0u         }}},
			{{{ UINT32_MAX, UINT32_MAX, 0u,         0u         }}},
			{{{ 0u,         0u,         UINT32_MAX, 0u         }}},
			{{{ UINT32_MAX, 0u,         UINT32_MAX, 0u         }}},
			{{{ 0u,         UINT32_MAX, UINT32_MAX, 0u         }}},
			{{{ UINT32_MAX, UINT32_MAX, UINT32_MAX, 0u         }}},
			{{{ 0u,         0u,         0u,         UINT32_MAX }}},
			{{{ UINT32_MAX, 0u,         0u,         UINT32_MAX }}},
			{{{ 0u,         UINT32_MAX, 0u,         UINT32_MAX }}},
			{{{ UINT32_MAX, UINT32_MAX, 0u,         UINT32_MAX }}},
			{{{ 0u,         0u,         UINT32_MAX, UINT32_MAX }}},
			{{{ UINT32_MAX, 0u,         UINT32_MAX, UINT32_MAX }}},
			{{{ 0u,         UINT32_MAX, UINT32_MAX, UINT32_MAX }}},
			{{{ UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX }}},
		};

		return (__m128)kMaskTable[ctrl];
	}

	// _mm_blend_psはマスクが即値じゃないといけないので、別途用意する. 
	inline __m128 _si_mm_blend(__si128 a, __si128 b, int ctrl)
	{
		__si128 mask = _si_mm_get_mask(ctrl);
		return _mm_or_ps(_mm_andnot_ps(mask, a), _mm_and_ps(b, mask));
	}
	inline __m128 _si_mm_blend(__si128 a, __si128 b, __si128 mask)
	{
		return _mm_or_ps(_mm_andnot_ps(mask, a), _mm_and_ps(b, mask));
	}

	// 順番が逆で分かりにくいので、別途用意する.
	inline __m128 _si_mm_set(float x, float y, float z, float w)
	{
		return _mm_set_ps(w, z, y, x);
	}


} // namespace SI
