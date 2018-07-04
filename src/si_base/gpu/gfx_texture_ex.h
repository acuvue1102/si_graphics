// テクスチャの種類別のクラス群
#pragma once

#include <cstdint>
#include "si_base/gpu/gfx_config.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_descriptor_heap.h"
#include "si_base/misc/reference_counter.h"
#include "si_base/gpu/gfx_color.h"
#include "si_base/gpu/gfx_gpu_resource.h"
#include "si_base/gpu/gfx_texture.h"

namespace SI
{
	class BaseTexture;

	enum class GfxTextureExType
	{
		Static = 0,
		Rt,
		DepthRt,
		Uav,
		RtUav,
		SwapChain
	};

	class GfxTextureEx : public GfxGpuResource
	{
	public:
		GfxTextureEx();
		virtual ~GfxTextureEx();
		
		void InitializeAs2DStatic( const char* name, uint32_t width, uint32_t height, GfxFormat format);
		void TerminateStatic();
		
		        const GfxDescriptor GetSrvDescriptor() const{ return m_srvDescriptor; }
		virtual const GfxDescriptor GetRtvDescriptor() const{ return GfxDescriptor(); }
		virtual const GfxDescriptor GetDsvDescriptor() const{ return GfxDescriptor(); }
		virtual const GfxDescriptor GetUavDescriptor() const{ return GfxDescriptor(); }
		
		virtual GfxResourceStates GetResourceStates() const{ return GfxResourceState::kCommon; }
		virtual void SetResourceStates(GfxResourceStates) {}
		
		virtual GfxTextureExType GetType() const
		{
			return GfxTextureExType::Static;
		}

		virtual GfxColorRGBA GetClearColor() const
		{
			return GfxColorRGBA();
		}
		
		virtual GfxDepthStencil GetClearDepthStencil() const
		{
			return GfxDepthStencil();
		}

		GfxTexture GetTexture()
		{
			return GfxTexture(m_texture);
		}
		
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		
		virtual void* GetNativeResource() override;
		
	public:
		BaseTexture*  GetBaseTexture(){ return m_texture; }

	protected:
		BaseTexture*            m_texture;
		GfxDescriptor           m_srvDescriptor;
		ReferenceCounter        m_ref;
		GfxDimension            m_dimension;
	};
	
	// 書き換えの可能性があるテクスチャの基底クラス
	class _GfxTextureEx_Writable : public GfxTextureEx
	{
	protected:
		_GfxTextureEx_Writable()
			: m_initialStates(GfxResourceState::kMax)
			, m_stateHandle(kInvalidHandle)
		{
		}

		virtual ~_GfxTextureEx_Writable()
		{
			SI_ASSERT(m_stateHandle == kInvalidHandle);
		}

	public:
		virtual GfxResourceStates GetInitialResourceStates() const override
		{
			return m_initialStates;
		}

		virtual uint32_t GetResourceStateHandle() const override
		{
			return m_stateHandle;
		}

		virtual void SetResourceStateHandle(uint32_t handle) override
		{
			m_stateHandle = handle;
		}

	protected:
		void SetInitialResourceStates(GfxResourceStates states)
		{
			m_initialStates = states;
		}

	protected:
		GfxResourceStates  m_initialStates;
		uint32_t           m_stateHandle;
	};

	class GfxTestureEx_Rt : public _GfxTextureEx_Writable
	{
	public:
		GfxTestureEx_Rt();
		virtual ~GfxTestureEx_Rt();
		
		void InitializeAs2DRt(
			const char* name, uint32_t width, uint32_t height,
			GfxFormat format, const GfxColorRGBA& clearColor = GfxColorRGBA());
		void TerminateRt();
		
		virtual GfxResourceStates GetResourceStates() const override
		{
			return m_resourceStates;
		}

		virtual void SetResourceStates(GfxResourceStates state) override
		{
			m_resourceStates = state;
		}
		
		virtual GfxTextureExType GetType() const override
		{
			return GfxTextureExType::Rt;
		}

		virtual GfxColorRGBA GetClearColor() const override
		{
			return m_clearColor;
		}

		virtual const GfxDescriptor GetRtvDescriptor() const override
		{
			return m_rtvDescriptor;
		}

	private:
		GfxDescriptor           m_rtvDescriptor;
		GfxResourceStates       m_resourceStates;
		GfxColorRGBA            m_clearColor;
	};
	
	class GfxTestureEx_DepthRt : public _GfxTextureEx_Writable
	{
	public:
		GfxTestureEx_DepthRt();
		virtual ~GfxTestureEx_DepthRt();
		
		void InitializeAs2DDepthRt(
			const char* name, uint32_t width, uint32_t height,
			const float rtClearDepth);
		
		void InitializeAs2DDepthStencilRt(
			const char* name, uint32_t width, uint32_t height,
			float clearDepth, uint8_t clearStencil);

		void TerminateDepthRt(); // + stencil
		
		virtual GfxResourceStates GetResourceStates() const override
		{
			return m_resourceStates;
		}

		virtual void SetResourceStates(GfxResourceStates state) override
		{
			m_resourceStates = state;
		}
		
		virtual GfxTextureExType GetType() const override
		{
			return GfxTextureExType::DepthRt;
		}

		virtual GfxDepthStencil GetClearDepthStencil() const override
		{
			return m_clearDepthStencil;
		}

		virtual const GfxDescriptor GetDsvDescriptor() const override
		{
			return m_dsvDescriptor;
		}

	private:
		GfxDescriptor           m_dsvDescriptor;
		GfxResourceStates       m_resourceStates;
		GfxDepthStencil         m_clearDepthStencil;
	};

	
	class GfxTestureEx_SwapChain : public _GfxTextureEx_Writable
	{
	public:
		GfxTestureEx_SwapChain();
		virtual ~GfxTestureEx_SwapChain();
		
		void InitializeAsSwapChain(
			const char* name, uint32_t width, uint32_t height,
			void* nativeSwapChain, uint32_t swapChainIndex);
		void TerminateSwapChain();
		
		virtual GfxResourceStates GetResourceStates() const override
		{
			return m_resourceStates;
		}

		virtual void SetResourceStates(GfxResourceStates state) override
		{
			m_resourceStates = state;
		}
		
		virtual GfxTextureExType GetType() const override
		{
			return GfxTextureExType::SwapChain;
		}

		virtual const GfxDescriptor GetRtvDescriptor() const override
		{
			return m_rtvDescriptor;
		}

		virtual GfxColorRGBA GetClearColor() const override
		{
			return m_clearColor;
		}

		// SwapChainのテクスチャは初期化時にクリアカラーを指定しないので、外から変えられるようにしておく.
		void SetClearColor(const GfxColorRGBA& color)
		{
			m_clearColor = color;
		}

	private:
		GfxDescriptor           m_rtvDescriptor;
		GfxResourceStates       m_resourceStates;
		GfxColorRGBA            m_clearColor;
	};

} // namespace SI
