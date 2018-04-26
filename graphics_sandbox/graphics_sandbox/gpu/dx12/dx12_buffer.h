#pragma once

#include "gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>

namespace SI
{
	class BaseBuffer
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseBuffer();
		~BaseBuffer();
	};

} // namespace SI

#endif // SI_USE_DX12
