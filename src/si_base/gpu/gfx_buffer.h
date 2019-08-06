#pragma once

#include <cstdint>
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseBuffer;
	struct GfxBufferDesc
	{
		const char*        m_name              = nullptr;
		size_t             m_bufferSizeInByte  = 0;
		GfxHeapType        m_heapType          = GfxHeapType::Default;
		GfxResourceStates  m_resourceStates    = GfxResourceState::CopyDest;
		GfxResourceFlags   m_resourceFlags     = GfxResourceFlag::None;
	};

	class GfxBuffer
	{
	public:
		GfxBuffer(BaseBuffer* base=nullptr)
			: m_base(base)
		{
		}

		~GfxBuffer()
		{
		}
		
		void* Map  (uint32_t subResourceId = 0);
		void  Unmap(uint32_t subResourceId = 0);
		size_t GetSize() const;
		GpuAddress GetGpuAddress() const;

	public:
		      BaseBuffer* GetBaseBuffer()      { return m_base; }
		const BaseBuffer* GetBaseBuffer() const{ return m_base; }

		bool IsValid() const{ return m_base != 0; }

	private:
		BaseBuffer* m_base;
	};

	class GfxIndexBufferView
	{
	public:
		GfxIndexBufferView()
			: m_buffer()
			, m_format(GfxFormat::R32_Uint)
			, m_sizeInByte(0)
			, m_offsetInByte(0)
		{}

		GfxIndexBufferView(
			GfxBuffer buffer,
			GfxFormat format,
			size_t    sizeInByte,
			size_t    offsetInByte = 0)
			: m_buffer(buffer)
			, m_sizeInByte(sizeInByte)
			, m_format(format)
			, m_offsetInByte(offsetInByte)
		{}
		
		inline void             SetBuffer(GfxBuffer buffer) { m_buffer = buffer; }
		inline void             SetFormat(GfxFormat format) { m_format = format; }
		inline void             SetSize(size_t size)        { m_sizeInByte   = size;   }
		inline void             SetOffset(size_t offset)    { m_offsetInByte = offset; }

		inline const GfxBuffer  GetBuffer() const{ return m_buffer; }
		inline GfxFormat        GetFormat() const{ return m_format; }
		inline size_t           GetSize()   const{ return m_sizeInByte;   }
		inline size_t           GetOffset() const{ return m_offsetInByte; }

	private:
		GfxBuffer  m_buffer;
		GfxFormat  m_format;
		size_t     m_sizeInByte;
		size_t     m_offsetInByte;
	};

	class GfxVertexBufferView
	{
	public:
		GfxVertexBufferView()
			: m_buffer(nullptr)
			, m_sizeInByte(0)
			, m_strideInByte(0)
			, m_offsetInByte(0)
		{}

		GfxVertexBufferView(
			GfxBuffer buffer,
			size_t sizeInByte,
			size_t strideInByte,
			size_t offsetInByte = 0)
			: m_buffer(buffer)
			, m_sizeInByte(sizeInByte)
			, m_strideInByte(strideInByte)
			, m_offsetInByte(offsetInByte)
		{}
		
		inline void             SetBuffer(GfxBuffer  buffer) { m_buffer = buffer;             }
		inline void             SetSize  (size_t sizeInByte) { m_sizeInByte = sizeInByte; }
		inline void             SetStride(size_t strideInByte) { m_strideInByte = strideInByte; }
		inline void             SetOffset(size_t offsetInByte) { m_offsetInByte = offsetInByte; }

		inline const GfxBuffer  GetBuffer() const{ return m_buffer; }
		inline size_t           GetSize()   const{ return m_sizeInByte; }
		inline size_t           GetStride() const{ return m_strideInByte; }
		inline size_t           GetOffset() const{ return m_offsetInByte; }

	private:
		GfxBuffer  m_buffer;
		size_t     m_sizeInByte;
		size_t     m_strideInByte;
		size_t     m_offsetInByte;
	};

} // namespace SI

