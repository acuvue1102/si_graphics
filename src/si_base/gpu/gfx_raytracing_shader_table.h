#pragma once

#include <vector>
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_declare.h"
#include "si_base/gpu/gfx_device_std_allocator.h"
#include "si_base/gpu/gfx_raytracing_shader_table.h"
#include "si_base/gpu/gfx_raytracing_state.h"

namespace SI
{
	class BaseRaytracingShaderTables;

	class GfxShaderRecord
	{
	public:
		GfxShaderRecord()
			: m_shaderId(nullptr)
			, m_localRootArgument(nullptr)
			, m_localRootArgumentSize(0)
		{
		}

		GfxShaderRecord(
			const void* shaderId,
			const void* localRootArgument = nullptr,
			size_t      localRootArgumentSize = 0)
			: m_shaderId(shaderId)
			, m_localRootArgument(localRootArgument)
			, m_localRootArgumentSize(localRootArgumentSize)
		{
		}

		const void* GetShaderId() const{ return m_shaderId; }
		const void* GetLocalRootArgument()     const{ return m_localRootArgument; }
		size_t      GetLocalRootArgumentSize() const{ return m_localRootArgumentSize; }

	private:
		const void* m_shaderId;
		const void* m_localRootArgument;
		size_t      m_localRootArgumentSize;
	};

	struct GfxShaderTableDesc
	{
		void SetShaderName(const char* shaderName){ m_shaderName = shaderName; }
		const char* GetShaderName() const{ return m_shaderName; }

		void ReserveRecord(size_t size)
		{
			m_shaderRecords.reserve(size);
		}

		void AddShaderRecord(const GfxShaderRecord& record)
		{
			m_maxLocalRootArgumentSize = SI::Max(m_maxLocalRootArgumentSize, record.GetLocalRootArgumentSize());
			m_shaderRecords.push_back(record);
		}

		template<typename T>
		void AddShaderRecord(GfxRaytracingState& state, const char* shaderName, const T& rootArgument)
		{
			AddShaderRecord(GfxShaderRecord(state.GetShaderId(shaderName), &rootArgument, sizeof(rootArgument)));
		}

		void AddShaderRecord(GfxRaytracingState& state, const char* shaderName)
		{
			AddShaderRecord(GfxShaderRecord(state.GetShaderId(shaderName)));
		}

		uint32_t GetShaderRecordCount() const
		{
			return (uint32_t)m_shaderRecords.size();
		}

		const GfxShaderRecord& GetShaderRecord(uint32_t index) const
		{
			return m_shaderRecords[index];
		}

		size_t GetMaxLocalRootArgumentSize() const
		{
			return m_maxLocalRootArgumentSize;
		}

	private:
		const char* m_shaderName    = nullptr;
		size_t m_maxLocalRootArgumentSize = 0;
		GfxTempVector<GfxShaderRecord>  m_shaderRecords;
	};

	struct GfxRaytracingShaderTablesDesc
	{
		void SetName(const char* name){ m_name = name; }
		const char* GetName() const{ return m_name; }

		void SetRaytracingState(GfxRaytracingState* state){ m_state = state; }
		GfxRaytracingState* GetRaytracingState(){ return m_state; }
		const GfxRaytracingState* GetRaytracingState() const{ return m_state; }

		      GfxShaderTableDesc& GetRayGenTableDesc()       { return m_rayGenTable; }
		const GfxShaderTableDesc& GetRayGenTableDesc() const { return m_rayGenTable; }
		      GfxShaderTableDesc& GetMissTableDesc()       { return m_missTable; }
		const GfxShaderTableDesc& GetMissTableDesc() const { return m_missTable; }
		      GfxShaderTableDesc& GetHitGroupTableDesc()      { return m_hitGroupTable; }
		const GfxShaderTableDesc& GetHitGroupTableDesc() const{ return m_hitGroupTable; }

	private:
		const char* m_name          = nullptr;
		GfxRaytracingState* m_state = nullptr;

		GfxShaderTableDesc m_rayGenTable;
		GfxShaderTableDesc m_missTable;
		GfxShaderTableDesc m_hitGroupTable;
	};

	class GfxRaytracingShaderTables
	{
	public:
		GfxRaytracingShaderTables()
			: m_base(nullptr)
		{
		}

		GfxRaytracingShaderTables(BaseRaytracingShaderTables* base)
			: m_base(base)
		{
		}

		      BaseRaytracingShaderTables* GetBase()      { return m_base; }
		const BaseRaytracingShaderTables* GetBase() const{ return m_base; }

	private:
		BaseRaytracingShaderTables* m_base;
	};

} // namespace SI
