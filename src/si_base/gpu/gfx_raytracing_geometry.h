#pragma once

#include <vector>
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_declare.h"
#include "si_base/gpu/gfx_buffer.h"

namespace SI
{
	class BaseRaytracingScene;

	struct GfxRaytracingGeometryTriangleDesc
	{
		GpuAddress                 m_transform3x4 = 0;
		GpuAddress                 m_indexBuffer  = 0;
		GpuAddress                 m_vertexBuffer = 0;
		uint64_t                   m_vertexStride = sizeof(float)*3;
		GfxFormat                  m_indexFormat  = GfxFormat::R16_Uint;
		GfxFormat                  m_vertexFormat = GfxFormat::R32G32B32_Float;
		uint32_t                   m_indexCount   = 0;
		uint32_t                   m_vertexCount  = 0;
	};

	struct GfxRaytracingGeometryProdeduralDesc
	{
		uint64_t                   m_aabbCount     = 0;
		GpuAddress                 m_startAddress  = 0;
		uint64_t                   m_strideInBytes = 0;
	};

	struct GfxRaytracingGeometryDesc
	{
		GfxRaytracingGeometryType m_type = GfxRaytracingGeometryType::Triangles;
		GfxRaytracingGeometryFlag m_flags = GfxRaytracingGeometryFlag::Opaque;
		//union 
		//{
			GfxRaytracingGeometryTriangleDesc    m_triangle;
			GfxRaytracingGeometryProdeduralDesc  m_aabbs;
		//};
	};

	struct GfxRaytracingASInputs
	{
		GfxRaytracingASType        m_type        = GfxRaytracingASType::TopLevel;
		GfxRaytracingASBuildFlags  m_flags       = GfxRaytracingASBuildFlag::PreferFastBuild;
		uint32_t                   m_descCount   = 0;
		GfxElementsLayout          m_descsLayout = GfxElementsLayout::Array;

		GpuAddress                              m_instanceDescs = 0;
		const GfxRaytracingGeometryDesc*        m_geometryDescs = nullptr;
		const GfxRaytracingGeometryDesc*const * m_geometryDescPointers = nullptr;
	};


	class GfxRaytracingScene
	{
	public:
		GfxRaytracingScene()
			: m_base(nullptr)
		{
		}

		GfxRaytracingScene(BaseRaytracingScene* base)
			: m_base(base)
		{
		}

		GfxBuffer GetTopASBuffer();
		GfxBuffer GetBottomASBuffer();

		      BaseRaytracingScene* GetBase()      { return m_base; }
		const BaseRaytracingScene* GetBase() const{ return m_base; }

	private:
		BaseRaytracingScene* m_base = nullptr;
	};

} // namespace SI
