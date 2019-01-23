
#include "si_base/renderer/material.h"

#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_graphics_state.h"
#include "si_base/gpu/gfx_input_layout.h"

namespace SI
{
	Material::Material()
		: m_name(kInvalidObjectIndex)
	{
	}
	
	Material::~Material()
	{
		m_shaderPS.Release();
		m_shaderVS.Release();
	}

	void Material::ImportSerializeData(const MaterialSerializeData& serializeData)
	{
		m_name = serializeData.m_name;
		
		std::string shaderPath = "asset\\shader\\lambert.hlsl";
		if(m_shaderVS.LoadAndCompile(shaderPath.c_str()) != 0)
		{
			SI_ASSERT(0);
			return;
		}

		if(m_shaderPS.LoadAndCompile(shaderPath.c_str()) != 0)
		{
			SI_ASSERT(0);
			return;
		}
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
