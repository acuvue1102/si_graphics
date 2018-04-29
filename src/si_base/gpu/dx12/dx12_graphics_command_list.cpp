
#include "si_base/gpu\gfx_config.h"

#if SI_USE_DX12

#include <dxgi1_4.h>
#include <comdef.h>
#include "si_base/core/base.h"
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/dx12/dx12_texture.h"
#include "si_base/gpu/dx12/dx12_graphics_state.h"
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"

namespace SI
{
	BaseGraphicsCommandList::BaseGraphicsCommandList()
	{
	}

	
	BaseGraphicsCommandList::~BaseGraphicsCommandList()
	{
		Terminate();
	}
	
	int BaseGraphicsCommandList::Initialize(ID3D12Device& device)
	{
		HRESULT hr = device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandAllocator", _com_error(hr).ErrorMessage());
			return -1;
		}

		hr = device.CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_graphicsCommandList));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateCommandList", _com_error(hr).ErrorMessage());
			return -1;
		}

		hr = m_graphicsCommandList->Close();
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error m_graphicsCommandList->Close", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}

	int BaseGraphicsCommandList::Terminate()
	{
		m_graphicsCommandList.Reset();
		m_commandAllocator.Reset();

		return 0;
	}

} // namespace SI

#endif // SI_USE_DX12
