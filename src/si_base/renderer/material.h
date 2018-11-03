#pragma once

#include "si_base/renderer/renderer_common.h"
#include "si_base/container/array.h"

namespace SI
{
	struct MaterialSerializeData
	{
		LongObjectIndex m_name;

		MaterialSerializeData()
			: m_name(kInvalidLongObjectIndex)
		{
		}

		SI_REFLECTION(
			SI::MaterialSerializeData,
			SI_REFLECTION_MEMBER_AS_TYPE(m_name, uint32_t))
	};

	class Material
	{
	public:
		Material();
		~Material();
		
		void ExportSerializeData(MaterialSerializeData& outData) const
		{
			outData.m_name = m_name;
		}
		
		void ImportSerializeData(const MaterialSerializeData& serializeData)
		{
			m_name = serializeData.m_name;
		}

	public:
		static Material* Create();
		static void Release(Material*& material);		

	private:
		friend class FbxParser;

	private:
		LongObjectIndex m_name;
	};

} // namespace SI
