#pragma once

#include <stdint.h>
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseBuffer;
	struct GfxBufferDesc
	{
		const char*  m_name              = nullptr;
		size_t       m_bufferSizeInByte  = 0;
		GfxHeapType  m_heapType          = GfxHeapType::kDefault;
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

	public:
		      BaseBuffer* GetBaseBuffer()      { return m_base; }
		const BaseBuffer* GetBaseBuffer() const{ return m_base; }

	private:
		BaseBuffer* m_base;
	};

	class GfxIndexBufferView
	{
	public:
		GfxIndexBufferView(
			GfxBuffer* buffer = nullptr,
			GfxFormat format = GfxFormat::kR32_Uint)
			: m_buffer(buffer)
			, m_format(format)
		{}
		
		inline void             SetBuffer(GfxBuffer* buffer) { m_buffer = buffer;             }
		inline void             SetFormat(GfxFormat format) { m_format = format; }

		inline const GfxBuffer* GetBuffer() const{ return m_buffer; }
		inline GfxFormat        GetFormat() const{ return m_format; }

	private:
		GfxBuffer* m_buffer;
		GfxFormat  m_format;
	};

	class GfxVertexBufferView
	{
	public:
		GfxVertexBufferView(
			GfxBuffer* buffer = nullptr,
			size_t strideInByte = 0)
			: m_buffer(buffer)
			, m_strideInByte(strideInByte)
		{}
		
		inline void             SetBuffer(GfxBuffer* buffer) { m_buffer = buffer;             }
		inline void             SetStride(size_t strideInByte) { m_strideInByte = strideInByte; }

		inline const GfxBuffer* GetBuffer() const{ return m_buffer; }
		inline size_t           GetStride() const{ return m_strideInByte; }

	private:
		GfxBuffer* m_buffer;
		size_t     m_strideInByte;
	};

} // namespace SI

