
#include "si_base/gpu/gfx_raytracing_state.h"

#include "si_base/gpu/dx12/dx12_raytracing_state.h"
#include "si_base/gpu/gfx_root_signature.h"

namespace SI
{
	void GfxDxilLibraryDesc::SetDxilLibrary(const GfxShaderCodeByte& dxilLibrary)
	{
		m_base->SetDxilLibrary(dxilLibrary);
	}

	GfxShaderCodeByte GfxDxilLibraryDesc::GetDxilLibrary() const
	{
		return m_base->GetDxilLibrary();
	}

	void GfxDxilLibraryDesc::AddExportDesc(
		const char*     name,
		const char*     exportToRename,
		GfxExportFlags  flags)
	{
		m_base->AddExportDesc(name, exportToRename, flags);
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	void GfxHitGroupDesc::SetHitGroupExport(const char* ex)
	{
		m_base->SetHitGroupExport(ex);
	}

	const char* GfxHitGroupDesc::GetHitGroupExport() const
	{
		return m_base->GetHitGroupExport();
	}

	void GfxHitGroupDesc::SetType(GfxHitGroupType type)
	{
		m_base->SetType(type);
	}

	GfxHitGroupType GfxHitGroupDesc::GetType() const
	{
		return m_base->GetType();
	}

	void GfxHitGroupDesc::SetAnyHitShaderImport(const char* im)
	{
		m_base->SetAnyHitShaderImport(im);
	}

	const char* GfxHitGroupDesc::GetAnyHitShaderImport() const
	{
		return m_base->GetAnyHitShaderImport();
	}

	void GfxHitGroupDesc::SetClosestHitShaderImport(const char* im)
	{
		m_base->SetClosestHitShaderImport(im);
	}

	const char* GfxHitGroupDesc::GetClosestHitShaderImport() const
	{
		return m_base->GetClosestHitShaderImport();
	}

	void GfxHitGroupDesc::SetIntersectionShaderImport(const char* im)
	{
		m_base->SetIntersectionShaderImport(im);
	}

	const char* GfxHitGroupDesc::GetIntersectionShaderImport() const
	{
		return m_base->GetIntersectionShaderImport();
	}
	
/////////////////////////////////////////////////////////////////////////////////////////

	void GfxSubObjectToExportAssosiation::SetSubObjectToAssociation(const GfxStateSubObject& subObject)
	{
		m_base->SetSubObjectToAssociate(subObject.GetBase());
	}

	void GfxSubObjectToExportAssosiation::AddExport(const char* exportStr)
	{
		m_base->AddExport(exportStr);
	}

/////////////////////////////////////////////////////////////////////////////////////////

	void GfxRaytracingShaderConfigDesc::SetMaxPayloadSizeInBytes(uint32_t size)
	{
		m_base->SetMaxPayloadSizeInBytes(size);
	}

	uint32_t GfxRaytracingShaderConfigDesc::GetMaxPayloadSizeInBytes() const
	{
		return m_base->GetMaxPayloadSizeInBytes();
	}

	void GfxRaytracingShaderConfigDesc::SetMaxAttributeSizeInBytes(uint32_t size)
	{
		m_base->SetMaxAttributeSizeInBytes(size);
	}

	uint32_t GfxRaytracingShaderConfigDesc::GetMaxAttributeSizeInBytes() const
	{
		return m_base->GetMaxAttributeSizeInBytes();
	}
	
/////////////////////////////////////////////////////////////////////////////////////////

	void GfxRaytracingPipelineConfigDesc::SetMaxTraceRecursionDepth(uint32_t d)
	{
		m_base->SetMaxTraceRecursionDepth(d);
	}

	uint32_t GfxRaytracingPipelineConfigDesc::GetMaxTraceRecursionDepth() const
	{
		return m_base->GetMaxTraceRecursionDepth();
	}

/////////////////////////////////////////////////////////////////////////////////////////

	void GfxRaytracingStateDesc::SetType(GfxStateObjectType type)
	{
		m_base->SetType(type);
	}

	GfxStateObjectType GfxRaytracingStateDesc::GetType()
	{
		return m_base->GetType();
	}

	void GfxRaytracingStateDesc::ReserveSubObject(uint32_t size)
	{
		m_base->ReserveSubObject(size);
	}

	GfxDxilLibraryDesc GfxRaytracingStateDesc::CreateDxilLibraryDesc()
	{
		return GfxDxilLibraryDesc(m_base->CreateDxilLibraryDesc());
	}

	GfxStateSubObject GfxRaytracingStateDesc::AddSubObject(GfxDxilLibraryDesc desc)
	{
		return GfxStateSubObject(m_base->AddSubObject(desc.GetBase()));
	}

	GfxHitGroupDesc GfxRaytracingStateDesc::CreateHitGroupDesc()
	{
		return GfxHitGroupDesc(m_base->CreateHitGroupDesc());
	}

	GfxStateSubObject GfxRaytracingStateDesc::AddSubObject(GfxHitGroupDesc desc)
	{
		return GfxStateSubObject(m_base->AddSubObject(desc.GetBase()));
	}

	GfxSubObjectToExportAssosiation GfxRaytracingStateDesc::CreateSubObjectToExportAssosiation()
	{
		return GfxSubObjectToExportAssosiation(m_base->CreateSubObjectToExportAssosiation());
	}

	GfxStateSubObject GfxRaytracingStateDesc::AddSubObject(GfxSubObjectToExportAssosiation assosiation)
	{
		return GfxStateSubObject(m_base->AddSubObject(assosiation.GetBase()));
	}

	GfxRaytracingShaderConfigDesc GfxRaytracingStateDesc::CreateRaytracingShaderConfigDesc()
	{
		return GfxRaytracingShaderConfigDesc(m_base->CreateRaytracingShaderConfigDesc());
	}
	
	GfxStateSubObject GfxRaytracingStateDesc::AddSubObject(GfxRaytracingShaderConfigDesc desc)
	{
		return GfxStateSubObject(m_base->AddSubObject(desc.GetBase()));
	}

	GfxRaytracingPipelineConfigDesc GfxRaytracingStateDesc::CreateRaytracingPipelineConfigDesc()
	{
		return GfxRaytracingPipelineConfigDesc(m_base->CreateRaytracingPipelineConfigDesc());
	}

	GfxStateSubObject GfxRaytracingStateDesc::AddSubObject(GfxRaytracingPipelineConfigDesc desc)
	{
		return GfxStateSubObject(m_base->AddSubObject(desc.GetBase()));
	}

	GfxStateSubObject GfxRaytracingStateDesc::CreateAndAddGlobalRootSignature(GfxRootSignature* rs)
	{
		return GfxStateSubObject(m_base->CreateAndAddGlobalRootSignature(rs->GetBaseRootSignature()));
	}

	GfxStateSubObject GfxRaytracingStateDesc::CreateAndAddLocalRootSignature(GfxRootSignature* rs)
	{
		return GfxStateSubObject(m_base->CreateAndAddLocalRootSignature(rs->GetBaseRootSignature()));
	}

	/////////////////////////////////////////////////////////////////

	void* GfxRaytracingState::GetShaderId(const char* shaderName)
	{
		return m_base->GetShaderId(shaderName);
	}

} // namespace SI
