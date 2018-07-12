﻿#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include "si_base/gpu/gfx_enum.h"

struct IDXGISwapChain3;

namespace SI
{
	struct GfxTextureDesc;

	class BaseTexture
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseTexture();
		~BaseTexture();

		int Initialize(ID3D12Device& device, const GfxTextureDesc& desc);

		int InitializeAsSwapChainTexture(
			uint32_t width,
			uint32_t height,
			void* nativeSwapChain,
			uint32_t swapChainBufferId);

		int Terminate();
		
		uint32_t GetWidth() const
		{
			return m_width;
		}

		uint32_t GetHeight() const
		{
			return m_height;
		}

		uint32_t GetDepth() const
		{
			return m_depth;
		}

		GfxFormat GetFormat() const
		{
			return m_format;
		}
		
		void SetWidth(uint32_t w)
		{
			m_width = w;
		}
		
		void SetHeight(uint32_t h)
		{
			m_height = h;
		}
		
		void SetDepth(uint32_t d)
		{
			m_depth = d;
		}
		
		void SetFormat(GfxFormat f)
		{
			m_format = f;
		}

	public:
		ComPtr<ID3D12Resource>& GetComPtrResource()
		{
			return m_resource;
		}

		ID3D12Resource* GetNativeResource()
		{
			return m_resource.Get();
		}

	private:
		ComPtr<ID3D12Resource>            m_resource;
		uint32_t                          m_width  = 0;
		uint32_t                          m_height = 0;
		uint32_t                          m_depth  = 0;
		GfxFormat                         m_format = GfxFormat::Unknown;
	};

} // namespace SI

#endif // SI_USE_DX12
