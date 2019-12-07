
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

		uint32_t white = 0xffffffff;
		m_whiteTex.InitializeAs2DStatic("white", 1, 1, GfxFormat::R8G8B8A8_Unorm, &white, sizeof(white));
	}

	void Renderer::Terminate()
	{
		SI_ASSERT(m_models.empty());

		m_whiteTex.TerminateStatic();
		m_models.clear();
		m_constantAllocator.Terminate();
	}
		
	void Renderer::Add(ScenesInstancePtr& modelInstance)
	{
		SI_ASSERT(m_models.find(modelInstance.get()) == m_models.end());
		m_models[modelInstance.get()] = modelInstance;
	}
		
	void Renderer::Remove(ScenesInstancePtr& modelInstance)
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
			ScenesInstancePtr& modelIns = pair.second;

			RendererDrawStageList& drawStageList = modelIns->GetDrawStageList();
			RendererDrawStage* drawStage = drawStageList.GetDrawStage(stageType);
			if(!drawStage) continue;

			uint32_t renderItemCount = (uint32_t)drawStage->m_renderItems.size();
			for(uint32_t ri=0; ri<renderItemCount; ++ri)
			{
				RenderItem& renderItem = drawStage->m_renderItems[ri];

				renderItem.SetupPSO(renderDescCopy);

				SI_ASSERT(renderItem.IsValid());
				
				Material& material = *renderItem.m_material;
				RenderMaterial& renderMaterial = *renderItem.m_renderMaterial;

				material.UpdateRenderMaterial(frameIndex, *renderItem.m_scenes, &renderMaterial);
		
				context.SetPipelineState(renderItem.m_graphicsState.Get());
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
					worldMatrixArray[i] = renderItem.m_worldMatrix;
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

				context.SetPrimitiveTopology(renderItem.m_subMesh->GetTopology());
				
				uint32_t vertexAttributeCount = (uint32_t)renderItem.m_vertexAttributes->size();
				for(uint32_t v=0; v<vertexAttributeCount; ++v)
				{
					int accessorId = (*renderItem.m_vertexAttributes)[v].m_accessorId;
					SI_ASSERT(0 <= accessorId);
					Accessor& accessor = renderItem.m_scenes->GetAccessor(accessorId);

					GfxVertexBufferView vertexView(
						accessor.GetBuffer(),
						accessor.GetSizeInByte(),
						GetFormatBits(accessor.GetFormat()) / 8);

					context.SetVertexBuffer(v, vertexView);
				}

				GfxIndexBufferView indexView(
					renderItem.m_indexAccessor->GetBuffer(),
					renderItem.m_indexAccessor->GetFormat(),
					renderItem.m_indexAccessor->GetSizeInByte());
				context.SetIndexBuffer(&indexView);

				context.DrawIndexedInstanced(renderItem.m_indexAccessor->GetCount(), instanceCount);
			}
		}
	}
	
} // namespace SI
