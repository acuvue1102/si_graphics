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
	struct GfxDdsMetaData;

	enum class GfxTextureExType
	{
		Invalid = 0,
		Static,
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
		
		        GfxDescriptor GetSrvDescriptor() const{ return m_srvDescriptor; }
		virtual GfxDescriptor GetRtvDescriptor() const{ return GfxDescriptor(); }
		virtual GfxDescriptor GetDsvDescriptor() const{ return GfxDescriptor(); }
		virtual GfxDescriptor GetUavDescriptor() const{ return GfxDescriptor(); }
		
		virtual GfxResourceStates GetResourceStates() const{ return GfxResourceState::Common; }
		virtual void SetResourceStates(GfxResourceStates) {}
		
		virtual GfxTextureExType GetType() const = 0;
		virtual void Terminate() = 0;

		virtual GfxColorRGBA GetClearColor() const
		{
			return GfxColorRGBA();
		}
		
		virtual GfxDepthStencil GetClearDepthStencil() const
		{
			return GfxDepthStencil();
		}
		
		      GfxTexture& Get()     { return m_texture; }
		const GfxTexture& Get()const{ return m_texture; }
		
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		GfxFormat GetFormat() const;
		uint32_t GetArraySize() const;
		uint32_t GetMipLevels() const;
		
		virtual void* GetNativeResource() override;
		bool IsValid() const{ return Get().IsValid(); }
		
	public:
		BaseTexture*  GetBaseTexture(){ return m_texture.GetBaseTexture(); }

	protected:
		GfxTexture              m_texture;
		GfxDescriptor           m_srvDescriptor;
		ReferenceCounter        m_ref;
		GfxDimension            m_dimension;
	};

	class GfxTextureEx_Static : public GfxTextureEx
	{
	public:
		GfxTextureEx_Static();
		virtual ~GfxTextureEx_Static();

		void InitializeAs2DStatic(
			const char* name,
			uint32_t width,
			uint32_t height,
			GfxFormat format,
			const void* imageData,
			size_t imageDataSize,
			uint32_t mipLevel=1);
		int  InitializeDDS(
			const char* name,
			const void* ddsBuffer,
			size_t ddsBufferSize,
			GfxDdsMetaData* outDdsMetaData = nullptr);
		int  InitializeWIC(
			const char* name,
			const void* wicBuffer,
			size_t wicBufferSize);
		void TerminateStatic();

		virtual GfxTextureExType GetType() const override
		{
			return GfxTextureExType::Static;
		}

		virtual void Terminate() override
		{
			TerminateStatic();
		}

	private:
	};
	
	// 書き換えの可能性があるテクスチャの基底クラス
	class _GfxTextureEx_Writable : public GfxTextureEx
	{
	protected:
		_GfxTextureEx_Writable()
			: m_initialStates(GfxResourceState::Max)
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

		virtual void Terminate() override
		{
			TerminateRt();
		}

		virtual GfxColorRGBA GetClearColor() const override
		{
			return m_clearColor;
		}

		virtual GfxDescriptor GetRtvDescriptor() const override
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

		virtual void Terminate() override
		{
			TerminateDepthRt();
		}

		virtual GfxDepthStencil GetClearDepthStencil() const override
		{
			return m_clearDepthStencil;
		}

		virtual GfxDescriptor GetDsvDescriptor() const override
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

		virtual void Terminate() override
		{
			TerminateSwapChain();
		}

		virtual GfxDescriptor GetRtvDescriptor() const override
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
	
	class GfxTestureEx_Uav : public _GfxTextureEx_Writable
	{
	public:
		GfxTestureEx_Uav();
		virtual ~GfxTestureEx_Uav();
		
		void InitializeAs2DUav(
			const char* name, uint32_t width, uint32_t height,
			GfxFormat format);
		void TerminateUav();
		
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
			return GfxTextureExType::Uav;
		}

		virtual void Terminate() override
		{
			TerminateUav();
		}

		virtual GfxDescriptor GetUavDescriptor() const override
		{
			return m_uavDescriptor;
		}

	private:
		GfxDescriptor           m_uavDescriptor;
		GfxResourceStates       m_resourceStates;
	};

} // namespace SI
