#pragma once

#include "si_base/renderer/renderer_common.h"

namespace SI
{
	struct MeshSerializeData
	{
		ObjectIndexRange m_submeshIndeces;

		SI_REFLECTION(
			SI::MeshSerializeData,
			SI_REFLECTION_MEMBER(m_submeshIndeces))
	};

	class Mesh
	{
	public:
		Mesh(){}
		~Mesh(){}

		void ExportSerializeData(MeshSerializeData& outData) const
		{
			outData.m_submeshIndeces = m_submeshIndeces;
		}

		void ImportSerializeData(const MeshSerializeData& serializeData)
		{
			m_submeshIndeces  = serializeData.m_submeshIndeces;
		}
		
	private:
		friend class FbxParser;

	private:
		ObjectIndexRange m_submeshIndeces; // Model::m_submeshのindex
	};

} // namespace SI
