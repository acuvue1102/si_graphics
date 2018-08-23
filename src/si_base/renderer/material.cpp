
#include "si_base/renderer/material.h"

namespace SI
{
	Material::Material()
		//: m_name(nullptr)
		: m_name(kInvalidObjectIndex)
	{
	}
	
	Material::~Material()
	{
	}

	Material* Material::Create()
	{
		return new Material();
	}

	void Material::Release(Material*& material)
	{
		delete material;
		material = nullptr;
	}

} // namespace SI
