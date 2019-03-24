
#include "si_base/renderer/material_factory.h"

#include "si_base/core/new_delete.h"
#include "si_base/renderer/material/material_simple.h"


namespace SI
{
	Material* MaterialFactory::Create(MaterialType materialType)
	{
		switch (materialType)
		{
		case SI::MaterialType::Invalid:
			break;
		case SI::MaterialType::Simple:
			return SI_NEW(MaterialSimple);
		default:
			break;
		}

		return nullptr;
	}

	void MaterialFactory::Release(Material* material)
	{
		SI_DELETE(material);
	}
} // namespace SI
