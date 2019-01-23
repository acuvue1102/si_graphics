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
		{
		}

		explicit GfxRootSignatureDescEx(uint32_t tableCount);

		~GfxRootSignatureDescEx()
		{
			Terminate();
		}

		void SetName(const char* name)
		{
			m_name = name;
		}

		//void InitiaizeWithShaderSet(const GfxShaderSet& shaderSet);

		// tableのメモリ開放.
		void Terminate();

		// tableのメモリ確保
		void ReserveTables(uint32_t tableCount);

		GfxDescriptorHeapTableEx& GetTable( uint32_t tableIndex )
		{
			SI_ASSERT(tableIndex < m_tableCount);
			return m_tables[tableIndex];
		}

		GfxRootSignatureDesc GetDesc() const
		{
			return GfxRootSignatureDesc(
				m_name,
				reinterpret_cast<GfxDescriptorHeapTable*>(m_tables),
				m_tableCount);
		}

		operator GfxRootSignatureDesc() const
		{
			return GetDesc();
		}

	private:
		const char*               m_name;
		GfxDescriptorHeapTableEx* m_tables;
		uint32_t                  m_tableCount;
	};

	class GfxRootSignatureEx
	{
	public:
		GfxRootSignatureEx()
		: m_samplerTableBits(0)
		, m_viewsTableBits(0)
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
		GfxRootSignature&       GetRootSignature(){ return m_sig; }
		const GfxRootSignature& GetRootSignature() const{ return m_sig; }
		operator GfxRootSignature() const{ return m_sig; }

		uint64_t GetSamplerTableBits()             const{ return m_samplerTableBits; };
		uint64_t GetViewsTableBits()               const{ return m_viewsTableBits;   };
		uint16_t GetTableDesciptorCount(uint32_t index) const
		{
			return m_tableDescriptorCount[index];
		}

	private:
		GfxRootSignature m_sig;
		ReferenceCounter m_ref;
		uint64_t m_samplerTableBits;                              // sampler tableの使われている箇所のbitを立てたtable.
		uint64_t m_viewsTableBits;                                // sampler以外のtableで使われている箇所のビットを立てたtable.
		uint16_t m_tableDescriptorCount[kMaxNumDescriptorTables]; // tableのdescriptorの数
	};



} // namespace SI