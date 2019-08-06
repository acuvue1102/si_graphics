#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include "si_base/gpu/gfx_enum.h"
#include "si_base/gpu/gfx_device_std_allocator.h"
#include "si_base/gpu/dx12/dx12_declare.h"
#include "si_base/gpu/dx12/dx12_buffer.h"

namespace SI
{
	class BaseRaytracingScene
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	public:
		BaseRaytracingScene();
		~BaseRaytracingScene();

		void Initialize(BaseDevice* baseDevice, size_t bottomASSize, size_t topASSize);
		void Release(BaseDevice* baseDevice);

	public:
		BaseBuffer* m_bottomLevelAccelerationStructure;
		BaseBuffer* m_topLevelAccelerationStructure;

		ComPtr<ID3D12Resource> m_scratchResource;
		ComPtr<ID3D12Resource> m_instanceDescs;
	};

} // namespace SI

#endif // SI_USE_DX12