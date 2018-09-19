#pragma once

#include "si_base/renderer/renderer_common.h"
#include "si_base/container/array.h"

namespace SI
{
	class Material
	{
	public:
		Material();
		~Material();

	public:
		static Material* Create();
		static void Release(Material*& material);		

	private:
		friend class FbxParser;

	private:
		ObjectIndex m_name;

		SI_REFLECTION(
			SI::Material,
			SI_REFLECTION_MEMBER_AS_TYPE(m_name, uint16_t))
	};

} // namespace SI
