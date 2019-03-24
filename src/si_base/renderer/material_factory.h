#pragma once

#include "si_base/renderer/material_type.h"

namespace SI
{
	class Material;

	class MaterialFactory
	{
	public:
		static Material* Create(MaterialType materialType);
		static void Release(Material* material);
	};

} // namespace SI
