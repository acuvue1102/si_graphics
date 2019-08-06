#pragma once

#include <vector>
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_declare.h"

namespace SI
{
	class BaseStateSubObject;
	class BaseDxilLibraryDesc;
	class BaseHitGroupDesc;
	class BaseSubObjectToExportAssosiation;
	class BaseRaytracingShaderConfigDesc;
	class BaseRaytracingPipelineConfigDesc;
	class BaseRaytracingStateDesc;
	class BaseRaytracingState;
	
	struct GfxDispatchRaysDesc
	{
		uint32_t m_width  = 1;
		uint32_t m_height = 1;
		uint32_t m_depth  = 1;

		const GfxRaytracingShaderTables* m_tables = nullptr;
	};

	struct GfxShaderCodeByte
	{
		const void*  m_shaderBytecode;
		size_t       m_bytecodeLength;

		GfxShaderCodeByte(
			const void*  shaderBytecode = nullptr,
			size_t       bytecodeLength = 0)
			: m_shaderBytecode(shaderBytecode)
			, m_bytecodeLength(bytecodeLength)
		{
		}
	};

	class GfxStateSubObject
	{
	public:
		GfxStateSubObject()
			: m_base(nullptr)
		{
		}

		GfxStateSubObject(BaseStateSubObject* base)
			: m_base(base)
		{
		}

	public:
		      BaseStateSubObject& GetBase()      { return *m_base; }
		const BaseStateSubObject& GetBase() const{ return *m_base; }

	private:
		BaseStateSubObject* m_base;
	};

	class GfxDxilLibraryDesc
	{
	public:
		GfxDxilLibraryDesc()
			: m_base(nullptr)
		{
		}

		GfxDxilLibraryDesc(BaseDxilLibraryDesc* base)
			: m_base(base)
		{
		}

		void SetDxilLibrary(const GfxShaderCodeByte& dxilLibrary);
		GfxShaderCodeByte GetDxilLibrary() const;

		void AddExportDesc(
			const char*     name,
			const char*     exportToRename= nullptr,
			GfxExportFlags  flags = GfxExportFlags::None);

	public:
		BaseDxilLibraryDesc& GetBase(){ return *m_base; }

	private:
		BaseDxilLibraryDesc* m_base;
	};

	class GfxHitGroupDesc
	{
	public:
		GfxHitGroupDesc()
			: m_base(nullptr)
		{
		}

		GfxHitGroupDesc(BaseHitGroupDesc* base)
			: m_base(base)
		{
		}

		void             SetHitGroupExport(const char* ex);
		const char*      GetHitGroupExport() const;

		void             SetType(GfxHitGroupType type);
		GfxHitGroupType  GetType() const;

		void             SetAnyHitShaderImport(const char* im);
		const char*      GetAnyHitShaderImport() const;

		void             SetClosestHitShaderImport(const char* im);
		const char*      GetClosestHitShaderImport() const;

		void             SetIntersectionShaderImport(const char* im);
		const char*      GetIntersectionShaderImport() const;

	public:
		BaseHitGroupDesc& GetBase(){ return *m_base; }

	private:
		BaseHitGroupDesc* m_base;
	};
	
	class GfxSubObjectToExportAssosiation
	{
	public:
		GfxSubObjectToExportAssosiation()
			: m_base(nullptr)
		{
		}

		GfxSubObjectToExportAssosiation(BaseSubObjectToExportAssosiation* base)
			: m_base(base)
		{
		}

		void SetSubObjectToAssociation(const GfxStateSubObject& subObject);
		void AddExport(const char* exportStr);

	public:
		BaseSubObjectToExportAssosiation& GetBase(){ return *m_base; }

	private:
		BaseSubObjectToExportAssosiation* m_base;
	};

	class GfxRaytracingShaderConfigDesc
	{
	public:
		GfxRaytracingShaderConfigDesc()
			: m_base(nullptr)
		{
		}

		GfxRaytracingShaderConfigDesc(BaseRaytracingShaderConfigDesc* base)
			: m_base(base)
		{
		}

		void SetMaxPayloadSizeInBytes(uint32_t size);
		uint32_t GetMaxPayloadSizeInBytes() const;

		void SetMaxAttributeSizeInBytes(uint32_t size);
		uint32_t GetMaxAttributeSizeInBytes() const;

	public:
		BaseRaytracingShaderConfigDesc& GetBase(){ return *m_base; }

	private:
		BaseRaytracingShaderConfigDesc* m_base;
	};

	class GfxRaytracingPipelineConfigDesc
	{
	public:
		GfxRaytracingPipelineConfigDesc()
			: m_base(nullptr)
		{
		}

		GfxRaytracingPipelineConfigDesc(BaseRaytracingPipelineConfigDesc* base)
			: m_base(base)
		{
		}

		void SetMaxTraceRecursionDepth(uint32_t d);
		uint32_t GetMaxTraceRecursionDepth() const;

	public:
		BaseRaytracingPipelineConfigDesc& GetBase(){ return *m_base; }

	private:
		BaseRaytracingPipelineConfigDesc* m_base;
	};

	class GfxRaytracingStateDesc
	{
	public:
		GfxRaytracingStateDesc()
			: m_base(nullptr)
		{
		}

		GfxRaytracingStateDesc(BaseRaytracingStateDesc* base)
			: m_base(base)
		{
		}

		void SetType(GfxStateObjectType type);
		GfxStateObjectType GetType();

		void ReserveSubObject(uint32_t size);

		GfxDxilLibraryDesc CreateDxilLibraryDesc();
		GfxStateSubObject AddSubObject(GfxDxilLibraryDesc desc);

		GfxHitGroupDesc CreateHitGroupDesc();
		GfxStateSubObject AddSubObject(GfxHitGroupDesc desc);

		GfxSubObjectToExportAssosiation CreateSubObjectToExportAssosiation();
		GfxStateSubObject AddSubObject(GfxSubObjectToExportAssosiation assosiation);

		GfxRaytracingShaderConfigDesc CreateRaytracingShaderConfigDesc();
		GfxStateSubObject AddSubObject(GfxRaytracingShaderConfigDesc desc);
		
		GfxRaytracingPipelineConfigDesc CreateRaytracingPipelineConfigDesc();
		GfxStateSubObject AddSubObject(GfxRaytracingPipelineConfigDesc desc);
		
		GfxStateSubObject CreateAndAddGlobalRootSignature(GfxRootSignature* rs);

		GfxStateSubObject CreateAndAddLocalRootSignature(GfxRootSignature* rs);

	public:
		BaseRaytracingStateDesc& GetBase(){ return *m_base; }

	private:
		BaseRaytracingStateDesc* m_base;
	};

	class GfxRaytracingState
	{
	public:
		GfxRaytracingState()
			: m_base(nullptr)
		{
		}

		GfxRaytracingState(BaseRaytracingState* base)
			: m_base(base)
		{
		}

		~GfxRaytracingState(){}

		void* GetShaderId(const char* shaderName);

	public:
		      BaseRaytracingState& GetBase()      { return *m_base; }
		const BaseRaytracingState& GetBase() const{ return *m_base; }

	private:
		BaseRaytracingState* m_base;
	};

} // namespace SI
