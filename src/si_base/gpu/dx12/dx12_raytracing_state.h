#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <string>
#include <array>
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_device_std_allocator.h"
#include "si_base/gpu/dx12/dx12_declare.h"
#include "si_base/gpu/gfx_raytracing_state.h"

namespace SI
{
	class BaseStateSubObject
	{
	public:
		BaseStateSubObject()
		{
			m_subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_MAX_VALID;
			m_subObject.pDesc = nullptr;
		}

		BaseStateSubObject(
			D3D12_STATE_SUBOBJECT_TYPE type,
			const void *pDesc)
		{
			m_subObject.Type = type;
			m_subObject.pDesc = pDesc;
		}

	public:
		      D3D12_STATE_SUBOBJECT& Get()      { return m_subObject; }
		const D3D12_STATE_SUBOBJECT& Get() const{ return m_subObject; }

	private:
		D3D12_STATE_SUBOBJECT m_subObject;
	};
	static_assert(sizeof(D3D12_STATE_SUBOBJECT) == sizeof(BaseStateSubObject), "size error");

	class BaseDxilLibraryDesc
	{
	public:
		BaseDxilLibraryDesc()
		{
			m_desc.DXILLibrary.pShaderBytecode = nullptr;
			m_desc.DXILLibrary.BytecodeLength = 0;
			m_desc.NumExports = 0;
			m_desc.pExports = nullptr;
		}

		~BaseDxilLibraryDesc();

		void SetDxilLibrary(const GfxShaderCodeByte& dxilLibrary);
		GfxShaderCodeByte GetDxilLibrary() const;

		void AddExportDesc(
			const char* name,
			const char* exportToRename,
			GfxExportFlags  flags);

	public:
		D3D12_DXIL_LIBRARY_DESC& Get();

	private:
		D3D12_DXIL_LIBRARY_DESC m_desc;

		GfxTempVector<D3D12_EXPORT_DESC> m_exports;
		GfxTempVector<GfxTempWString*>   m_strings;
	};

	//////////////////////////////////////////////////////////////////////////////

	class BaseHitGroupDesc
	{
	public:
		BaseHitGroupDesc()
		{
			m_desc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
			m_desc.AnyHitShaderImport = nullptr;
			m_desc.ClosestHitShaderImport = nullptr;
			m_desc.HitGroupExport= nullptr;
			m_desc.IntersectionShaderImport = nullptr;

			m_orgStrings.fill(nullptr);
		}

		void             SetType(GfxHitGroupType type);
		GfxHitGroupType  GetType() const;

		void             SetHitGroupExport(const char* ex);
		const char*      GetHitGroupExport() const;

		void             SetAnyHitShaderImport(const char* im);
		const char*      GetAnyHitShaderImport() const;

		void             SetClosestHitShaderImport(const char* im);
		const char*      GetClosestHitShaderImport() const;

		void             SetIntersectionShaderImport(const char* im);
		const char*      GetIntersectionShaderImport() const;

	public:
		D3D12_HIT_GROUP_DESC& Get(){ return m_desc; }
	
	private:
		D3D12_HIT_GROUP_DESC m_desc;

		GfxTempWString m_strings[4];
		std::array<const char*, 4> m_orgStrings;
	};

	//////////////////////////////////////////////////////////////////////////////

	class BaseSubObjectToExportAssosiation
	{
	public:
		BaseSubObjectToExportAssosiation()
		{
			m_association = {};
		}

		~BaseSubObjectToExportAssosiation();

		void SetSubObjectToAssociate(const BaseStateSubObject& subObjectToAssociate);
		void AddExport(const char* exportStr);

	public:
		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION& Get(){ return m_association; }

	private:
		D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION m_association;

		GfxTempVector<LPCWSTR>           m_exports;
		GfxTempVector<GfxTempWString*>   m_strings;
	};

	//////////////////////////////////////////////////////////////////////////////

	class BaseRaytracingShaderConfigDesc
	{
	public:
		BaseRaytracingShaderConfigDesc()
		{
			m_desc.MaxAttributeSizeInBytes = 0;
			m_desc.MaxPayloadSizeInBytes = 0;
		}

		void SetMaxPayloadSizeInBytes(uint32_t size)
		{
			m_desc.MaxPayloadSizeInBytes = size;
		}

		uint32_t GetMaxPayloadSizeInBytes() const
		{
			return m_desc.MaxPayloadSizeInBytes;
		}

		void SetMaxAttributeSizeInBytes(uint32_t size)
		{
			m_desc.MaxAttributeSizeInBytes = size;
		}

		uint32_t GetMaxAttributeSizeInBytes() const
		{
			return m_desc.MaxAttributeSizeInBytes;
		}

	public:
		D3D12_RAYTRACING_SHADER_CONFIG& Get(){ return m_desc; }

	private:
		D3D12_RAYTRACING_SHADER_CONFIG m_desc;
	};

	//////////////////////////////////////////////////////////////////////////////

	class BaseRaytracingPipelineConfigDesc
	{
	public:
		BaseRaytracingPipelineConfigDesc()
		{
			m_desc.MaxTraceRecursionDepth = 1;
		}

		void SetMaxTraceRecursionDepth(uint32_t d)
		{
			m_desc.MaxTraceRecursionDepth = d;
		}

		uint32_t GetMaxTraceRecursionDepth() const
		{
			return m_desc.MaxTraceRecursionDepth;
		}

	public:
		D3D12_RAYTRACING_PIPELINE_CONFIG& Get(){ return m_desc; }

	private:
		D3D12_RAYTRACING_PIPELINE_CONFIG m_desc;
	};

	//////////////////////////////////////////////////////////////////////////////

	class BaseRaytracingStateDesc
	{
	public:
		BaseRaytracingStateDesc();
		~BaseRaytracingStateDesc();

		void SetType(GfxStateObjectType type);
		GfxStateObjectType GetType();

		void ReserveSubObject(uint32_t size);

		BaseDxilLibraryDesc* CreateDxilLibraryDesc();
		BaseStateSubObject* AddSubObject(BaseDxilLibraryDesc& desc);

		BaseHitGroupDesc* CreateHitGroupDesc();
		BaseStateSubObject* AddSubObject(BaseHitGroupDesc& desc);

		BaseSubObjectToExportAssosiation* CreateSubObjectToExportAssosiation();
		BaseStateSubObject* AddSubObject(BaseSubObjectToExportAssosiation& assosiation);

		BaseRaytracingShaderConfigDesc* CreateRaytracingShaderConfigDesc();
		BaseStateSubObject* AddSubObject(BaseRaytracingShaderConfigDesc& desc);

		BaseRaytracingPipelineConfigDesc* CreateRaytracingPipelineConfigDesc();
		BaseStateSubObject* AddSubObject(BaseRaytracingPipelineConfigDesc& desc);

		BaseStateSubObject* CreateAndAddGlobalRootSignature(BaseRootSignature* rs);

		BaseStateSubObject* CreateAndAddLocalRootSignature(BaseRootSignature* rs);

	public:
		D3D12_STATE_OBJECT_DESC& Get();

	private:
		D3D12_STATE_OBJECT_DESC m_desc;

		GfxTempVector<BaseStateSubObject> m_subObjects;
		GfxTempVector<BaseRaytracingPipelineConfigDesc*> m_raytracingPipelineConfigs;
		GfxTempVector<BaseSubObjectToExportAssosiation*> m_subObjectToExportAssociations;
		GfxTempVector<BaseRaytracingShaderConfigDesc*> m_raytracingShaderConfigs;
		GfxTempVector<BaseHitGroupDesc*> m_hitGroupDescs;
		GfxTempVector<BaseDxilLibraryDesc*> m_dxilLibraryDescs;
		uint32_t m_rootSignatureCount;
	};

	class BaseRaytracingState
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		BaseRaytracingState();
		~BaseRaytracingState();

		int Initialize(ID3D12Device5& device, BaseRaytracingStateDesc& desc);
		
		void* GetShaderId(const char* shaderName);

		ComPtr<ID3D12StateObject>& GetComPtrState(){ return m_dxrStateObject; }

	public:
		ComPtr<ID3D12StateObject> m_dxrStateObject;
	};

} // namespace SI

#endif // SI_USE_DX12