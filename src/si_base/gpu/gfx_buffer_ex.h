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
	};
	
	class GfxBufferEx : public GfxGpuResource
	{
	public:
		GfxBufferEx();
		virtual ~GfxBufferEx();
		
		void InitializeAsStatic( const char* name, size_t size);
		void TerminateStatic();
		
		virtual GfxDescriptor GetSrvDescriptor() const{ return GfxDescriptor(); }
		virtual GfxDescriptor GetUavDescriptor() const{ return GfxDescriptor(); }
		
		virtual GfxResourceStates GetResourceStates() const{ return GfxResourceState::kCommon; }
		virtual void SetResourceStates(GfxResourceStates) {}
		
		virtual GfxBufferExType GetType() const
		{
			return GfxBufferExType::Static;
		}
		
		GfxBuffer GetBuffer()
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
		void TerminateConstant();
		
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

} // namespace SI
