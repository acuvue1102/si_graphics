
#include <comdef.h>
#include "base/base.h"
#include "gpu/dx12/dx12_fence.h"

#if SI_USE_DX12
namespace SI
{	
	BaseFenceEvent::BaseFenceEvent()
		: m_fenceEvent(0)
	{
	}

	BaseFenceEvent::~BaseFenceEvent()
	{
		Terminate();
	}
	
	int BaseFenceEvent::Initialize()
	{
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if(m_fenceEvent == nullptr)
		{
			HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
			if(FAILED(hr))
			{
				SI_ASSERT(0, "error CreateEvent", _com_error(hr).ErrorMessage());
				return -1;
			}
		}

		return 0;
	}
	
	int BaseFenceEvent::Terminate()
	{
		if(m_fenceEvent)
		{
			CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////

	BaseFence::BaseFence()
	{
	}

	BaseFence::~BaseFence()
	{
		Terminate();
	}

	
	int BaseFence::Initialize(ID3D12Device& device)
	{
		HRESULT hr = device.CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		if(FAILED(hr))
		{
			SI_ASSERT(0, "error CreateFence", _com_error(hr).ErrorMessage());
			return -1;
		}

		return 0;
	}

	int BaseFence::Terminate()
	{
		m_fence.Reset();
		return 0;
	}


} // namespace SI

#endif // SI_USE_DX12
