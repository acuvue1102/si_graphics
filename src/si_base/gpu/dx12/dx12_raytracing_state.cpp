
#include "si_base/gpu/dx12/dx12_raytracing_state.h"

#include <d3d12.h>

#include "si_base/gpu/dx12/dx12_enum.h"
#include "si_base/gpu/dx12/dx12_root_signature.h"
#include "si_base/gpu/dx12/dx12_device.h"

namespace SI
{
	namespace
	{
		void SetWString(GfxTempWString& outStr, const char* str)
		{
			size_t len = strlen(str);

			outStr.resize(len+1);

			size_t ret;
			wchar_t* buf = &outStr[0];
			mbstowcs_s(&ret, buf, outStr.size(), str, len);
		}
	}

	////////////////////////////////////////////////////////////////////////////

	BaseDxilLibraryDesc::~BaseDxilLibraryDesc()
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();

		for(GfxTempWString* str : m_strings)
		{
			allocator.Delete(str);
		}
		m_strings.clear();
	}

	void BaseDxilLibraryDesc::SetDxilLibrary(const GfxShaderCodeByte& dxilLibrary)
	{
		m_desc.DXILLibrary.BytecodeLength = dxilLibrary.m_bytecodeLength;
		m_desc.DXILLibrary.pShaderBytecode = dxilLibrary.m_shaderBytecode;
	}

	GfxShaderCodeByte BaseDxilLibraryDesc::GetDxilLibrary() const
	{
		GfxShaderCodeByte ret;
		ret.m_bytecodeLength = m_desc.DXILLibrary.BytecodeLength;
		ret.m_shaderBytecode = m_desc.DXILLibrary.pShaderBytecode;
		return ret;
	}

	void BaseDxilLibraryDesc::AddExportDesc(
		const char* name,
		const char* exportToRename,
		GfxExportFlags  flags)
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();

		D3D12_EXPORT_DESC& exportDesc = m_exports.emplace_back();

		GfxTempWString* newName = allocator.New<GfxTempWString>();
		m_strings.emplace_back(newName);
		SetWString(*newName, name);
		exportDesc.Name = newName->c_str();

		if(exportToRename)
		{
			GfxTempWString* newExportToRename = allocator.New<GfxTempWString>();
			m_strings.emplace_back(newExportToRename);
			SetWString(*newExportToRename, exportToRename);
			exportDesc.ExportToRename = newExportToRename->c_str();
		}
		else
		{
			exportDesc.ExportToRename = nullptr;
		}

		exportDesc.Flags = GetDx12ExportFlags(flags);
	}

	D3D12_DXIL_LIBRARY_DESC& BaseDxilLibraryDesc::Get()
	{
		m_desc.NumExports = (UINT)m_exports.size();
		m_desc.pExports = m_exports.data();

		return m_desc;
	}

	////////////////////////////////////////////////////////////////////////////

	void BaseHitGroupDesc::SetType(GfxHitGroupType type)
	{
		m_desc.Type = GetDx12HitGroupType(type);
	}

	GfxHitGroupType  BaseHitGroupDesc::GetType() const
	{
		// TODO: fixit. 遅いけど、使うことないだろうからいいだろう...
		for(uint32_t i=0; i<(uint32_t)GfxHitGroupType::Max; ++i)
		{
			if(m_desc.Type == GetDx12HitGroupType((GfxHitGroupType)i))
			{
				return (GfxHitGroupType)i;
			}
		}

		return GfxHitGroupType::Max;
	}

	void BaseHitGroupDesc::SetHitGroupExport(const char* ex)
	{
		SetWString(m_strings[0], ex);
		m_desc.HitGroupExport = m_strings[0].c_str();
		m_orgStrings[0] = ex;
	}

	const char* BaseHitGroupDesc::GetHitGroupExport() const
	{
		return m_orgStrings[0];
	}

	void BaseHitGroupDesc::SetAnyHitShaderImport(const char* im)
	{
		SetWString(m_strings[1], im);
		m_desc.AnyHitShaderImport = m_strings[1].c_str();
		m_orgStrings[1] = im;
	}

	const char* BaseHitGroupDesc::GetAnyHitShaderImport() const
	{
		return m_orgStrings[1];
	}

	void BaseHitGroupDesc::SetClosestHitShaderImport(const char* im)
	{
		SetWString(m_strings[2], im);
		m_desc.ClosestHitShaderImport = m_strings[2].c_str();
		m_orgStrings[2] = im;
	}

	const char* BaseHitGroupDesc::GetClosestHitShaderImport() const
	{
		return m_orgStrings[2];
	}

	void BaseHitGroupDesc::SetIntersectionShaderImport(const char* im)
	{
		SetWString(m_strings[3], im);
		m_desc.IntersectionShaderImport = m_strings[3].c_str();
		m_orgStrings[3] = im;
	}

	const char* BaseHitGroupDesc::GetIntersectionShaderImport() const
	{
		return m_orgStrings[3];
	}

	////////////////////////////////////////////////////////////////////////////

	BaseSubObjectToExportAssosiation::~BaseSubObjectToExportAssosiation()
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();

		for(GfxTempWString* str : m_strings)
		{
			allocator.Delete(str);
		}
		m_strings.clear();
	}

	void BaseSubObjectToExportAssosiation::SetSubObjectToAssociate(const BaseStateSubObject& subObjectToAssociate)
	{
		m_association.pSubobjectToAssociate = &(subObjectToAssociate.Get());
	}

	void BaseSubObjectToExportAssosiation::AddExport(const char* exportStr)
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();
		GfxTempWString* str = m_strings.emplace_back(allocator.New<GfxTempWString>());

		SetWString(*str, exportStr);
		m_exports.emplace_back(str->c_str());

		m_association.NumExports = (UINT)m_exports.size();
		m_association.pExports   = m_exports.data();
	}
	
	////////////////////////////////////////////////////////////////////////////

	BaseRaytracingStateDesc::BaseRaytracingStateDesc()
		: m_rootSignatureCount(0)
	{
		m_desc.NumSubobjects = 0;
		m_desc.pSubobjects = nullptr;
		m_desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	}

	BaseRaytracingStateDesc::~BaseRaytracingStateDesc()
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();

		for(BaseDxilLibraryDesc* desc : m_dxilLibraryDescs)
		{
			allocator.Delete(desc);
		}
		m_dxilLibraryDescs.clear();

		for(BaseHitGroupDesc* desc : m_hitGroupDescs)
		{
			allocator.Delete(desc);
		}
		m_hitGroupDescs.clear();

		for(BaseRaytracingShaderConfigDesc* config : m_raytracingShaderConfigs)
		{
			allocator.Delete(config);
		}
		m_raytracingShaderConfigs.clear();

		for(BaseSubObjectToExportAssosiation* assosiation : m_subObjectToExportAssociations)
		{
			allocator.Delete(assosiation);
		}
		m_subObjectToExportAssociations.clear();

		for(BaseRaytracingPipelineConfigDesc* config : m_raytracingPipelineConfigs)
		{
			allocator.Delete(config);
		}
		m_raytracingPipelineConfigs.clear();

		m_subObjects.clear();
	}

	void BaseRaytracingStateDesc::SetType(GfxStateObjectType type)
	{
		m_desc.Type = GetDx12StateObjectType(type);
	}
	
	GfxStateObjectType BaseRaytracingStateDesc::GetType()
	{
		// TODO: fixit. 遅いけど、使うことないだろうからいいだろう...
		for(uint32_t i=0; i<(uint32_t)GfxStateObjectType::Max; ++i)
		{
			if(m_desc.Type == GetDx12StateObjectType((GfxStateObjectType)i))
			{
				return (GfxStateObjectType)i;
			}
		}
		
		return GfxStateObjectType::Max;
	}

	void BaseRaytracingStateDesc::ReserveSubObject(uint32_t size)
	{
		m_subObjects.reserve(size);
	}
	
	BaseDxilLibraryDesc* BaseRaytracingStateDesc::CreateDxilLibraryDesc()
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();
		BaseDxilLibraryDesc& desc = *allocator.New<BaseDxilLibraryDesc>();
		m_dxilLibraryDescs.emplace_back(&desc);

		return &desc;
	}

	BaseStateSubObject* BaseRaytracingStateDesc::AddSubObject(BaseDxilLibraryDesc& desc)
	{
		SI_ASSERT(m_subObjects.size() < m_subObjects.capacity(), "途中でサイズが変わるとまずい");

		BaseStateSubObject& newSubObject = m_subObjects.emplace_back(
			D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY,
			&desc.Get());

		return &newSubObject;
	}

	BaseHitGroupDesc* BaseRaytracingStateDesc::CreateHitGroupDesc()
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();
		BaseHitGroupDesc& desc = *allocator.New<BaseHitGroupDesc>();

		m_hitGroupDescs.emplace_back(&desc);
		return &desc;
	}

	BaseStateSubObject* BaseRaytracingStateDesc::AddSubObject(BaseHitGroupDesc& desc)
	{
		SI_ASSERT(m_subObjects.size() < m_subObjects.capacity(), "途中でサイズが変わるとまずい");

		BaseStateSubObject& newSubObject = m_subObjects.emplace_back(
			D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP,
			&desc.Get());

		return &newSubObject;
	}

	BaseSubObjectToExportAssosiation* BaseRaytracingStateDesc::CreateSubObjectToExportAssosiation()
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();
		BaseSubObjectToExportAssosiation& association = *allocator.New<BaseSubObjectToExportAssosiation>();

		m_subObjectToExportAssociations.emplace_back(&association);
		return &association;
	}

	BaseStateSubObject* BaseRaytracingStateDesc::AddSubObject(BaseSubObjectToExportAssosiation& assosiation)
	{
		SI_ASSERT(m_subObjects.size() < m_subObjects.capacity(), "途中でサイズが変わるとまずい");

		BaseStateSubObject& newSubObject = m_subObjects.emplace_back(
			D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION,
			&assosiation.Get());

		return &newSubObject;
	}

	BaseRaytracingShaderConfigDesc* BaseRaytracingStateDesc::CreateRaytracingShaderConfigDesc()
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();
		BaseRaytracingShaderConfigDesc& config = *allocator.New<BaseRaytracingShaderConfigDesc>();

		m_raytracingShaderConfigs.emplace_back(&config);
		return &config;
	}

	BaseStateSubObject* BaseRaytracingStateDesc::AddSubObject(BaseRaytracingShaderConfigDesc& desc)
	{
		SI_ASSERT(m_subObjects.size() < m_subObjects.capacity(), "途中でサイズが変わるとまずい");

		BaseStateSubObject& newSubObject = m_subObjects.emplace_back(
			D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG,
			&desc.Get());

		return &newSubObject;
	}

	BaseRaytracingPipelineConfigDesc* BaseRaytracingStateDesc::CreateRaytracingPipelineConfigDesc()
	{
		PoolAllocatorEx& allocator = SI_DEVICE_TEMP_ALLOCATOR();
		BaseRaytracingPipelineConfigDesc& config = *allocator.New<BaseRaytracingPipelineConfigDesc>();

		m_raytracingPipelineConfigs.emplace_back(&config);
		return &config;
	}

	BaseStateSubObject* BaseRaytracingStateDesc::AddSubObject(BaseRaytracingPipelineConfigDesc& desc)
	{
		SI_ASSERT(m_subObjects.size() < m_subObjects.capacity(), "途中でサイズが変わるとまずい");

		BaseStateSubObject& newSubObject = m_subObjects.emplace_back(
			D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG,
			&desc.Get());

		return &newSubObject;
	}

	BaseStateSubObject* BaseRaytracingStateDesc::CreateAndAddGlobalRootSignature(BaseRootSignature* rs)
	{
		SI_ASSERT(m_subObjects.size() < m_subObjects.capacity(), "途中でサイズが変わるとまずい");

		ID3D12RootSignature** rootSignature = rs->GetComPtrRootSignature().GetAddressOf();

		BaseStateSubObject& newSubObject = m_subObjects.emplace_back(
			D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE,
			rootSignature);

		++m_rootSignatureCount;

		return &newSubObject;
	}

	BaseStateSubObject* BaseRaytracingStateDesc::CreateAndAddLocalRootSignature(BaseRootSignature* rs)
	{
		SI_ASSERT(m_subObjects.size() < m_subObjects.capacity(), "途中でサイズが変わるとまずい");

		ID3D12RootSignature** rootSignature = rs->GetComPtrRootSignature().GetAddressOf();

		BaseStateSubObject& newSubObject = m_subObjects.emplace_back(
			D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE,
			rootSignature);

		++m_rootSignatureCount;

		return &newSubObject;
	}

	D3D12_STATE_OBJECT_DESC& BaseRaytracingStateDesc::Get()
	{
		SI_ASSERT(m_subObjects.size() == 
			(m_raytracingPipelineConfigs.size() +
			m_subObjectToExportAssociations.size() +
			m_raytracingShaderConfigs.size() +
			m_hitGroupDescs.size() +
			m_dxilLibraryDescs.size() +
			m_rootSignatureCount),
			"何かAddSubObjectし忘れてる."
		);

		m_desc.NumSubobjects = (UINT)m_subObjects.size();
		m_desc.pSubobjects = &m_subObjects.data()->Get();

		return m_desc;
	}

	////////////////////////////////////////////////////////////////////////////

	BaseRaytracingState::BaseRaytracingState()
	{
	}

	BaseRaytracingState::~BaseRaytracingState()
	{
	}

	int BaseRaytracingState::Initialize(ID3D12Device5& device, BaseRaytracingStateDesc& desc)
	{
		HRESULT hr = device.CreateStateObject(&desc.Get(), IID_PPV_ARGS(&m_dxrStateObject));

		return SUCCEEDED(hr) ? 0 : -1;
	}

	void* BaseRaytracingState::GetShaderId(const char* shaderName)
	{
		ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
		HRESULT hr = m_dxrStateObject.As(&stateObjectProperties);
		SI_ASSERT(SUCCEEDED(hr));

		GfxTempWString wshaderName;
		SetWString(wshaderName, shaderName);
		return stateObjectProperties->GetShaderIdentifier(wshaderName.c_str());
	}

} // namespace SI
