#pragma once

#include <cstdint>
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/core/assert.h"
#include "si_base/misc/reference_counter.h"
#include "si_base/gpu/gfx_root_signature.h"

namespace SI
{
	class GfxShader;

	// GfxDescriptorHeapTableだけだと不便なので、便利にしたクラス.
	class GfxDescriptorHeapTableEx : private NonCopyable
	{
	public:
		GfxDescriptorHeapTableEx()
			: m_table()
		{
		}
		
		~GfxDescriptorHeapTableEx()
		{
			Terminate();
		}

		void Terminate();

		void ReserveRanges(uint32_t rangeCount, GfxShaderVisibility visibility = GfxShaderVisibility::All);

		GfxDescriptorRange& GetRange(uint32_t rangeIndex)
		{
			SI_ASSERT(rangeIndex < m_table.m_rangeCount);
			return m_table.m_ranges[rangeIndex];
		}

		GfxDescriptorRange& operator[](size_t rangeIndex)
		{
			SI_ASSERT(rangeIndex < (size_t)m_table.m_rangeCount);
			return m_table.m_ranges[rangeIndex];
		}

	private:
		GfxDescriptorHeapTable m_table;
	};
	static_assert(sizeof(GfxDescriptorHeapTable)==sizeof(GfxDescriptorHeapTableEx), "size error");

	struct GfxShaderSet
	{
		const GfxShader* m_vertexShader = nullptr;
		const GfxShader* m_pixelShader  = nullptr;
	};

	// GfxRootSignatureDescだけだと不便なので、便利にしたクラス.
	class GfxRootSignatureDescEx : private NonCopyable
	{		
	public:
		GfxRootSignatureDescEx()
			: m_name(nullptr)
			, m_tables(nullptr)
			, m_tableCount(0)
			, m_rootDescriptors(0)
			, m_rootDescriptorCount(0)
		{
		}

		GfxRootSignatureDescEx(uint32_t tableCount, uint32_t rootDescriptorCount);

		~GfxRootSignatureDescEx()
		{
			Terminate();
		}

		void SetName(const char* name)
		{
			m_name = name;
		}

		//void InitiaizeWithShaderSet(const GfxShaderSet& shaderSet);

		// table/descriptorのメモリ開放.
		void Terminate();

		// tableのメモリ確保
		void CreateTables(uint32_t tableCount);

		// descriptorのメモリ確保
		void CreateRootDescriptors(uint32_t descriptorCount);

		GfxDescriptorHeapTableEx& GetTable( uint32_t tableIndex )
		{
			SI_ASSERT(tableIndex < m_tableCount);
			return m_tables[tableIndex];
		}

		uint32_t GetTableCount() const{ return m_tableCount; }

		GfxRootDescriptor& GetRootDescriptor( uint32_t rootDescriptorIndex )
		{
			SI_ASSERT(rootDescriptorIndex < m_rootDescriptorCount);
			return m_rootDescriptors[rootDescriptorIndex];
		}

		GfxRootSignatureDesc GetDesc() const
		{
			return GfxRootSignatureDesc(
				m_name,
				reinterpret_cast<GfxDescriptorHeapTable*>(m_tables),
				m_tableCount,
				m_rootDescriptors,
				m_rootDescriptorCount);
		}

		operator GfxRootSignatureDesc() const
		{
			return GetDesc();
		}

	private:
		const char*               m_name;
		GfxDescriptorHeapTableEx* m_tables;
		uint32_t                  m_tableCount;
		GfxRootDescriptor*        m_rootDescriptors;
		uint32_t                  m_rootDescriptorCount; // rootに入れるdescriptor.
	};

	class GfxRootSignatureEx
	{
	public:
		GfxRootSignatureEx()
		: m_samplerTableBits(0)
		, m_viewsTableBits(0)
		, m_tableCount(0)
		, m_rootDescriptorCount(0)
		{
			memset(m_tableDescriptorCount, 0, sizeof(m_tableDescriptorCount));
		}

		~GfxRootSignatureEx()
		{
			Terminate();
		}

		void Initialize(const GfxRootSignatureDesc& desc);
		void Terminate();

	public:
		GfxRootSignature&       Get(){ return m_sig; }
		const GfxRootSignature& Get() const{ return m_sig; }
		operator GfxRootSignature() const{ return m_sig; }

		uint64_t GetSamplerTableBits()             const{ return m_samplerTableBits; };
		uint64_t GetViewsTableBits()               const{ return m_viewsTableBits;   };
		uint16_t GetTableDesciptorCount(uint32_t index) const
		{
			return m_tableDescriptorCount[index];
		}
		
		uint32_t GetTableCount() const{ return m_tableCount; }
		uint32_t GetRootDescriptorCount() const{ return m_rootDescriptorCount; }

	private:
		GfxRootSignature m_sig;
		ReferenceCounter m_ref;
		uint64_t m_samplerTableBits;                              // sampler tableの使われている箇所のbitを立てたtable.
		uint64_t m_viewsTableBits;                                // sampler以外のtableで使われている箇所のビットを立てたtable.
		uint16_t m_tableDescriptorCount[kMaxNumDescriptorTables]; // tableのdescriptorの数

		uint32_t m_tableCount;
		uint32_t m_rootDescriptorCount;
	};



} // namespace SI