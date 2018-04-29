#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>

namespace SI
{
	class BaseCommandList
	{
	public:
		BaseCommandList(){}
		virtual ~BaseCommandList(){}

		virtual ID3D12CommandList* GetCommandList() = 0;
	};

} // namespace SI

#endif // SI_USE_DX12
