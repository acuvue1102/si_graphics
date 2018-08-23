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
	};

} // namespace SI
