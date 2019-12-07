#pragma once

#include <memory>
#include <vector>
#include "si_base/core/assert.h"
#include "si_base/misc/string.h"
#include "si_base/renderer/submesh.h"

namespace SI
{
	class Mesh
	{
	public:
		Mesh(){}
		~Mesh()
		{
			for(SubMesh* subMesh : m_subMeshes)
			{
				SI_DELETE(subMesh);
			}
			m_subMeshes.clear();
		}

		void SetName(const char* name){ m_name = name; }
		const char* GetName() const{ return m_name.c_str(); }

		void ReserveSubmeshes(size_t subMeshCount){ m_subMeshes.reserve(subMeshCount); }
		SubMesh* CreateNewSubMesh(){ m_subMeshes.push_back(SI_NEW(SubMesh)); return m_subMeshes.back(); }

		uint32_t GetSubMeshCount() const{ return (uint32_t)m_subMeshes.size(); }
		SubMesh* GetSubMesh(uint32_t id){ return m_subMeshes[id]; }
		const SubMesh* GetSubMesh(uint32_t id)const{ return m_subMeshes[id]; }

	private:
		String m_name;
		std::vector<SubMesh*> m_subMeshes;
	};

} // namespace SI
