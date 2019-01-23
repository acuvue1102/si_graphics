﻿
#include "si_base/gpu/dx12/dx12_upload_pool.h"

#if SI_USE_DX12

#include <dxgi1_4.h>
#include <comdef.h>
#include "si_base/core/core.h"
#include "si_base/gpu/dx12/dx12_graphics_command_list.h"

namespace SI
{
	template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	struct BaseUploadBuffer
	{
		BaseBuffer*                                       m_targetBuffer;
		ComPtr<ID3D12Resource>                            m_uploadbuffer;
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> m_layouts;

		BaseUploadBuffer(
			BaseBuffer*             targetBuffer,
			ComPtr<ID3D12Resource>  uploadbuffer,
			GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layout)
			: m_targetBuffer(targetBuffer)
			, m_uploadbuffer(std::move(uploadbuffer))
			, m_layouts(std::move(layout))
		{
		}
	};

	struct BaseUploadTexture
	{
		BaseTexture*                                      m_targetTexture;
		ComPtr<ID3D12Resource>                            m_uploadbuffer;
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> m_layouts;

		BaseUploadTexture(
			BaseTexture*            targetTexture,
			ComPtr<ID3D12Resource>  uploadbuffer,
			GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layout)
			: m_targetTexture(targetTexture)
			, m_uploadbuffer(std::move(uploadbuffer))
			, m_layouts(std::move(layout))
		{
		}
	};

	class BaseUploadPoolImpl
	{
	public:
		BaseUploadPoolImpl()
		{
		}

		~BaseUploadPoolImpl()
		{
		}

		void AddBuffer(
			BaseBuffer&                                        targetBuffer,
			ComPtr<ID3D12Resource>&                            uploadbuffer,
			GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& layouts)
		{
			MutexLocker locker(m_mutex);
			m_uploadBuffers.emplace_back(&targetBuffer, std::move(uploadbuffer), std::move(layouts));
		}

		void AddTexture(
			BaseTexture&                                       targetTexture,
			ComPtr<ID3D12Resource>&                            uploadbuffer,
			GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>& layouts)
		{
			MutexLocker locker(m_mutex);
			m_uploadTextures.emplace_back(&targetTexture, std::move(uploadbuffer), std::move(layouts));
		}

		void Flush(BaseGraphicsCommandList& graphicsCommandList)
		{
			// 貯めたアップロードリクエストをクリアする.
			MutexLocker locker(m_mutex);
			for(BaseUploadBuffer& b : m_uploadBuffers)
			{
				graphicsCommandList.UploadBuffer(
					*b.m_targetBuffer,
					b.m_uploadbuffer,
					b.m_layouts);
			}
			m_uploadBuffers.clear();
			
			for(BaseUploadTexture& t : m_uploadTextures)
			{
				graphicsCommandList.UploadTexture(
					*t.m_targetTexture,
					t.m_uploadbuffer,
					t.m_layouts);
			}
			m_uploadTextures.clear();
		}
	private:		
		std::vector<BaseUploadBuffer>  m_uploadBuffers;
		std::vector<BaseUploadTexture> m_uploadTextures;
		Mutex                          m_mutex;
	};


	/////////////////////////////
	
	BaseUploadPool::BaseUploadPool()
		: m_impl(SI_NEW(BaseUploadPoolImpl))
	{
	}

	BaseUploadPool::~BaseUploadPool()
	{
		SI_DELETE(m_impl);
	}

	void BaseUploadPool::AddBuffer(
		BaseBuffer& targetBuffer,
		ComPtr<ID3D12Resource> uploadbuffer,
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts)
	{
		m_impl->AddBuffer(targetBuffer, uploadbuffer, layouts);
	}

	void BaseUploadPool::AddTexture(
		BaseTexture& targetTexture,
		ComPtr<ID3D12Resource> uploadbuffer,
		GfxTempVector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts)
	{
		m_impl->AddTexture(targetTexture, uploadbuffer, layouts);
	}

	void BaseUploadPool::Flush(BaseGraphicsCommandList& graphicsCommandList)
	{
		m_impl->Flush(graphicsCommandList);
	}

} // namespace SI

#endif // SI_USE_DX12