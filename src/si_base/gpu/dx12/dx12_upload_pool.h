#pragma once

#include "si_base/gpu/gfx_config.h"

#if SI_USE_DX12
#include <d3d12.h>
#include <wrl/client.h>
#include <cstdint>
#include <vector>
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseBuffer;
	class BaseTexture;
	class PoolAllocatorEx;
	class BaseGraphicsCommandList;
	class BaseUploadPoolImpl;
	struct GfxCpuDescriptor;
	
	struct BaseUploadBuffer;
	struct BaseUploadTexture;

	template<typename T> class GfxStdDeviceTempAllocator;
	template<typename T> using GfxTempVector = std::vector<T, GfxStdDeviceTempAllocator<T>>;

	class BaseUploadPool
	{
		template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	public:
		BaseUploadPool();
		~BaseUploadPool();

		void AddBuffer(
			BaseBuffer& targetBuffer,
			ComPtr<ID3D12Resource> uploadbuffer,
			GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts,
			GfxResourceStates before,
			GfxResourceStates after);

		void AddTexture(
			BaseTexture& targetTexture,
			ComPtr<ID3D12Resource> uploadbuffer,
			GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts,
			GfxResourceStates before,
			GfxResourceStates after);

		void Flush(BaseGraphicsCommandList& graphicsCommandList);
		
	private:
		BaseUploadPoolImpl* m_impl; // ヘッダの依存解決が面倒だったので、implにする.
	};

} // namespace SI

#endif
