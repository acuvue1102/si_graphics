
#include "si_base/gpu/gfx_context_manager.h"

#include "si_base/gpu/gfx_command_queue.h"
#include "si_base/gpu/gfx_device.h"
#include "si_base/gpu/gfx_core.h"

namespace SI
{
	GfxContextManager::GfxContextManager()
	{
	}

	GfxContextManager::~GfxContextManager()
	{
	}
	
	void GfxContextManager::Initialize(const GfxContextManagerDesc& desc)
	{
		SI_ASSERT(m_graphicsContexts.size()==0);
		SI_ASSERT(m_graphicsCoordinators.size()==0);
		SI_ASSERT(0 < desc.m_contextCount);

		GfxDevice& device = SI_DEVICE();

		m_graphicsContexts.resize(desc.m_contextCount);
		for(GfxGraphicsContext*& context : m_graphicsContexts)
		{
			context = SI_NEW(GfxGraphicsContext);
			context->Initialize();
		}
		m_graphicsCoordinators.resize(desc.m_contextCount);
		for(GfxGraphicsCommandList*& commandList : m_graphicsCoordinators)
		{
			commandList = SI_NEW(GfxGraphicsCommandList);
			*commandList = device.CreateGraphicsCommandList();
		}

		m_tempCommandLists.reserve(desc.m_contextCount * 2);
	}

	void GfxContextManager::Terminate()
	{
		GfxDevice& device = SI_DEVICE();

		for(GfxGraphicsCommandList*& commandList : m_graphicsCoordinators)
		{
			device.ReleaseGraphicsCommandList(*commandList);
			SI_DELETE(commandList);
		}
		m_graphicsCoordinators.clear();

		
		for(GfxGraphicsContext* context : m_graphicsContexts)
		{
			context->Terminate();
			SI_DELETE(context);
		}
		m_graphicsContexts.clear();
	}
	
	void GfxContextManager::ResetContexts()
	{
		uint32_t contextCount = (uint32_t)m_graphicsContexts.size();
		for(uint32_t i=0; i<contextCount; ++i)
		{
			GfxGraphicsContext& context = *m_graphicsContexts[i];
			context.Reset();
		}
	}
	
	void GfxContextManager::CloseContexts()
	{
		uint32_t contextCount = (uint32_t)m_graphicsContexts.size();
		for(uint32_t i=0; i<contextCount; ++i)
		{
			GfxGraphicsContext& context = *m_graphicsContexts[i];
			context.Close();
		}
	}
	
	void GfxContextManager::Execute(GfxCommandQueue& queue)
	{
		SI_ASSERT(0 < m_graphicsCoordinators.size());

		m_tempCommandLists.clear();
		
		GfxResourceStatesPool& resourceStatePool = SI_RESOURCE_STATES_POOL();
		uint32_t maxAllocatedStateCount = resourceStatePool.GetMaxAllocatedStateCount();
		
		uint32_t contextCount = (uint32_t)m_graphicsContexts.size();
		for(uint32_t i=0; i<contextCount; ++i)
		{
			GfxGraphicsCommandList& coordinateCommandList = *m_graphicsCoordinators[i];
			GfxGraphicsContext&     context               = *m_graphicsContexts[i];
			coordinateCommandList.Reset(nullptr);

			for(uint32_t st=0; st<maxAllocatedStateCount; ++st)
			{
				GfxResourceStates pendding = context.GetPenddingResourceState(st);

				if(pendding == GfxResourceState::Pendding) continue; // このcontext内で変更してない
			
				GfxResourceStates prev = resourceStatePool.GetResourceStates(st);
				GfxResourceStates next = context.GetCurrentResourceState(st);

				if(prev == pendding) // context間で変更がない.
				{
					resourceStatePool.SetResourceStates(st, next); // 今のステートを保存.
					continue;
				}

				GfxGpuResource* res = resourceStatePool.GetGpuResource(st);
				coordinateCommandList.ResourceBarrier(*res, prev, pendding); // context間のステート調整.
				
				resourceStatePool.SetResourceStates(st, next); // 今のステートを保存.
			}

			coordinateCommandList.Close();
		}

		uint32_t count = (uint32_t)m_graphicsContexts.size();
		for(uint32_t i=0; i<count; ++i)
		{
			m_tempCommandLists.push_back( m_graphicsCoordinators[i] );

			m_tempCommandLists.push_back( m_graphicsContexts[i]->GetGraphicsCommandList() );
		}

		queue.ExecuteCommandLists((uint32_t)m_tempCommandLists.size(), &m_tempCommandLists[0]);
	}

} // namespace SI
