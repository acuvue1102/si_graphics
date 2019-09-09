
#include "si_base/renderer/renderer.h"

#include "si_base/math/vfloat4x4.h"
#include "si_base/gpu/gfx_graphics_context.h"

namespace SI
{
	struct SceneCB
	{
		Vfloat4x4 m_view;
		Vfloat4x4 m_proj;
		Vfloat4x4 m_viewProj;
	};

	///////////////////////////////////////////////////////////////////////////

	Renderer::Renderer()
		: Singleton<Renderer>(this)
		, m_frameIndex(0)
	{
	}
	
	Renderer::~Renderer()
	{
	}
	
	void Renderer::Initialize()
	{
		m_constantAllocator.Initialize(true);
	}

	void Renderer::Terminate()
	{
		m_models.clear();
		m_constantAllocator.Terminate();
	}
		
	void Renderer::Add(ModelInstancePtr& modelInstance)
	{
		SI_ASSERT(m_models.find(modelInstance.get()) == m_models.end());
		m_models[modelInstance.get()] = modelInstance;
	}
		
	void Renderer::Remove(ModelInstancePtr& modelInstance)
	{
		SI_ASSERT(m_models.find(modelInstance.get()) != m_models.end());
		m_models.erase(modelInstance.get());
	}

	void Renderer::Update()
	{
		++m_frameIndex; // flip. 64bitだから、桁あふれ気にしない.
		m_constantAllocator.Reset();
	}

	void Renderer::Render(
		GfxGraphicsContext& context,
		RendererDrawStageType stageType,
		const RendererGraphicsStateDesc& renderDesc)
	{
		RendererGraphicsStateDesc renderDescCopy = renderDesc;
		renderDescCopy.GenerateHash();

		uint32_t frameIndex = GetWriteFrameIndex();
			
		GfxLinearAllocatorMemory constant0 = m_constantAllocator.Allocate(sizeof(SceneCB), 256);
		SceneCB* sceneCB = (SceneCB*)constant0.GetCpuAddr();
		sceneCB->m_view     = m_viewMatrix;
		sceneCB->m_proj     = m_projectionMatrix;
		sceneCB->m_viewProj = m_viewMatrix * m_projectionMatrix;
		size_t constant0GpuAddr = constant0.GetGpuAddr();

		for(auto& pair : m_models)
		{
			ModelInstancePtr& modelIns = pair.second;

			RendererDrawStageList& drawStageList = modelIns->GetDrawStageList();
			RendererDrawStage* drawStage = drawStageList.GetDrawStage(stageType);
			if(!drawStage) continue;

			uint32_t renderItemCount = drawStage->GetRenderItemCount();
			for(uint32_t ri=0; ri<renderItemCount; ++ri)
			{
				RenderItem& renderItem = drawStage->GetRenderItem(ri);

				renderItem.SetupPSO(renderDescCopy);

				SI_ASSERT(renderItem.IsValid());
				
				Material& material = renderItem.GetMaterial();
				RenderMaterial& renderMaterial = renderItem.GetRenderMaterial();
				Geometry& geometry = renderItem.GetGeometry();
				ObjectIndex nodeIndex = renderItem.GetNodeIndex();
				SI_ASSERT(nodeIndex < modelIns->GetNodeCores().GetItemCount());
				const NodeCore& nodeCore = modelIns->GetNodeCores()[nodeIndex];

				material.UpdateRenderMaterial(renderMaterial, frameIndex);
		
				context.SetPipelineState(renderItem.GetGraphicsState().Get());
				context.SetGraphicsRootSignature(renderMaterial.GetRootSignature());
				
				GfxDescriptorHeap& srvHeap = renderMaterial.GetSrvHeap(frameIndex).Get();
				GfxDescriptorHeap& samplerHeap = renderMaterial.GetSamplerHeap(frameIndex).Get();

				context.SetDescriptorHeaps(
					&srvHeap,
					&samplerHeap);

				uint32_t instanceCount = 1;
				GfxLinearAllocatorMemory constant1 = m_constantAllocator.Allocate(instanceCount*sizeof(Vfloat4x4), 256);
				Vfloat4x4* worldMatrixArray = (Vfloat4x4*)constant1.GetCpuAddr();
				for(uint32_t i=0; i<instanceCount; ++i)
				{
					worldMatrixArray[i] = nodeCore.GetLocalMatrix();//Vfloat4x4::Identity();
				}
				size_t constant1GpuAddr = constant1.GetGpuAddr();

				GfxBuffer& constant2 = renderMaterial.GetConstantBuffer(frameIndex).Get();
				
				if(srvHeap.IsValid())
				{
					context.SetGraphicsDescriptorTable(0, srvHeap.GetGpuDescriptor(0));
				}
				
				if(samplerHeap.IsValid())
				{
					context.SetGraphicsDescriptorTable(1, samplerHeap.GetGpuDescriptor(0));
				}
				
				// コンスタントバッファをセットする.
				uint32_t cbvRootIndexOffset = renderMaterial.GetRootSignature().GetTableCount();
				context.SetGraphicsRootCBV(cbvRootIndexOffset  , constant0GpuAddr);
				context.SetGraphicsRootCBV(cbvRootIndexOffset+1, constant1GpuAddr);
				if(constant2.IsValid())
				{
					context.SetGraphicsRootCBV(cbvRootIndexOffset+2, constant2);
				}

				context.SetPrimitiveTopology(GfxPrimitiveTopology::TriangleList);
				
				context.SetVertexBuffer(0, geometry.GetVertexBuffer());
				context.SetIndexBuffer(geometry.GetIndexBuffer());
			
				context.DrawIndexedInstanced(geometry.GetIndexCount(), 1);
			}
		}
	}
	
} // namespace SI
