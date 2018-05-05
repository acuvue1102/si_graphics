#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>

namespace SI
{
	class BaseDevice;
	struct GfxRootSignatureDesc;

	class BaseRootSignature
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		BaseRootSignature();
		~BaseRootSignature();

		int Initialize(BaseDevice& device, const GfxRootSignatureDesc& desc);
		int Terminate();

	public:
		ComPtr<ID3D12RootSignature>& GetComPtrRootSignature()
		{
			return m_rootSignature;
		}

	private:
		ComPtr<ID3D12RootSignature> m_rootSignature;
	};

} // namespace SI

#endif // SI_USE_DX12
