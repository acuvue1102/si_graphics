#pragma once

#include "si_base/renderer/renderer_common.h"

namespace SI
{
	class Mesh
	{
	public:
		Mesh(){}
		~Mesh(){}
		
	private:
		friend class FbxParser;

	private:
		ObjectIndexRange m_submeshIndeces; // Model::m_submeshのindex

		SI_REFLECTION(
			SI::Mesh,
			SI_REFLECTION_MEMBER(m_submeshIndeces))
	};

} // namespace SI
