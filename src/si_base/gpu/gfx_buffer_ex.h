// バッファの種類別のクラス群
#pragma once

#include <cstdint>
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/misc/reference_counter.h"
#include "si_base/gpu/gfx_color.h"
#include "si_base/gpu/gfx_gpu_resource.h"
#include "si_base/gpu/gfx_buffer.h"

namespace SI
{
	class BaseBuffer;

	enum class GfxBufferExType
	{
		Static = 0,
		Constant,
		Index,
		Vertex
	};
	
	class GfxBufferEx : public GfxGpuResource
	{
	public:
		GfxBufferEx();
		virtual ~GfxBufferEx();
		
		void InitializeAsStatic( const char* name, size_t size);
		void TerminateAsStatic();
		
		virtual GfxDescriptor GetSrvDescriptor() const{ return GfxDescriptor(); }
		virtual GfxDescriptor GetUavDescriptor() const{ return GfxDescriptor(); }
		
		virtual GfxResourceStates GetResourceStates() const{ return GfxResourceState::Common; }
		virtual void SetResourceStates(GfxResourceStates) {}
		
		virtual GfxBufferExType GetType() const
		{
			return GfxBufferExType::Static;
		}
		
		GfxBuffer GetBuffer() const
		{
			return GfxBuffer(m_buffer);
		}
		
		size_t GetSize() const;
		virtual void* GetNativeResource() override;
		
	public:
		BaseBuffer*  GetBaseBuffer(){ return m_buffer; }

	protected:
		BaseBuffer*             m_buffer;
		ReferenceCounter        m_ref;
	};

	class GfxBufferEx_Constant : public GfxBufferEx
	{
	public:
		GfxBufferEx_Constant();
		virtual ~GfxBufferEx_Constant();
		
		void InitializeAsConstant( const char* name, size_t size);
		void TerminateAsConstant();
		
		virtual GfxDescriptor GetSrvDescriptor() const override{ return m_srvDescriptor; }
		
		virtual GfxBufferExType GetType() const override
		{
			return GfxBufferExType::Constant;
		}
		
		void* GetMapPtr()
		{
			return m_mapPtr;
		}

	protected:
		void*                   m_mapPtr;
		GfxDescriptor           m_srvDescriptor;
	};
	
	class GfxBufferEx_Index : public GfxBufferEx
	{
	public:
		GfxBufferEx_Index();
		virtual ~GfxBufferEx_Index();
		
		void InitializeAsIndex( const char* name, size_t indexCount, bool is16bit);
		void TerminateAsIndex();
		
		virtual GfxBufferExType GetType() const override
		{
			return GfxBufferExType::Index;
		}

		GfxFormat GetFormat() const{ return m_format; }

	protected:
		GfxFormat m_format;
	};
	
	class GfxBufferEx_Vertex : public GfxBufferEx
	{
	public:
		GfxBufferEx_Vertex();
		virtual ~GfxBufferEx_Vertex();
		
		void InitializeAsVertex( const char* name, size_t vertexCount, size_t stride, size_t offset);
		void TerminateAsVertex();
		
		virtual GfxBufferExType GetType() const override
		{
			return GfxBufferExType::Vertex;
		}
		
		size_t GetStride() const{ return m_stride; }
		size_t GetOffset() const{ return m_offset; }
		void SetOffset(size_t o){ m_offset = o; }

	protected:
		size_t m_stride;
		size_t m_offset;
	};

} // namespace SI
