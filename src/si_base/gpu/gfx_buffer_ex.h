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
		
		virtual GfxDescriptor GetSrvDescriptor() const{ return GfxDescriptor(); }
		virtual GfxDescriptor GetUavDescriptor() const{ return GfxDescriptor(); }
		
		virtual GfxResourceStates GetResourceStates() const{ return GfxResourceState::Common; }
		virtual void SetResourceStates(GfxResourceStates) {}
		
		virtual GfxBufferExType GetType() const = 0;
		virtual void Terminate() = 0;
		
		      GfxBuffer& Get()      { return m_buffer; }
		const GfxBuffer& Get() const{ return m_buffer; }
		
		size_t GetSize() const;
		virtual void* GetNativeResource() override;
		
	public:
		BaseBuffer*  GetBaseBuffer(){ return m_buffer.GetBaseBuffer(); }

	protected:
		GfxBuffer               m_buffer;
		ReferenceCounter        m_ref;
	};

	class GfxBufferEx_Static : public GfxBufferEx
	{
	public:
		GfxBufferEx_Static();
		virtual ~GfxBufferEx_Static();
		
		void InitializeAsStatic( const char* name, size_t size);
		void TerminateAsStatic();
		
		virtual GfxBufferExType GetType() const override
		{
			return GfxBufferExType::Static;
		}

		virtual void Terminate() override
		{
			TerminateAsStatic();
		}

	private:
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

		virtual void Terminate() override
		{
			TerminateAsConstant();
		}
		
		void* GetMapPtr()
		{
			return m_mapPtr;
		}
		
		size_t GetSize() const
		{
			return m_size;
		}

	protected:
		void*                   m_mapPtr;
		size_t                  m_size;
		GfxDescriptor           m_srvDescriptor;
	};
	
	class GfxBufferEx_Index : public GfxBufferEx
	{
	public:
		GfxBufferEx_Index();
		virtual ~GfxBufferEx_Index();
		
		void InitializeAsIndex(
			const char* name,
			const void* indexAddress,
			size_t indexCount,
			bool is16bit);
		void TerminateAsIndex();
		
		virtual GfxBufferExType GetType() const override
		{
			return GfxBufferExType::Index;
		}

		virtual void Terminate() override
		{
			TerminateAsIndex();
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
		
		void InitializeAsVertex(
			const char* name,
			const void* vertexAddress,
			size_t vertexCount,
			size_t stride,
			size_t offset);
		void TerminateAsVertex();
		
		virtual GfxBufferExType GetType() const override
		{
			return GfxBufferExType::Vertex;
		}

		virtual void Terminate() override
		{
			TerminateAsVertex();
		}
		
		size_t GetStride() const{ return m_stride; }
		size_t GetOffset() const{ return m_offset; }
		void SetOffset(size_t o){ m_offset = o; }

	protected:
		size_t m_stride;
		size_t m_offset;
	};

} // namespace SI
