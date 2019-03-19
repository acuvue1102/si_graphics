#pragma once

#include "si_base/container/array.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_buffer_ex.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/renderer/renderer_common.h"
#include "si_base/renderer/renderer_draw_stage_type.h"

namespace SI
{
	struct MaterialSerializeData
	{
		LongObjectIndex m_name;

		MaterialSerializeData()
			: m_name(kInvalidLongObjectIndex)
		{
		}

		SI_REFLECTION(
			SI::MaterialSerializeData,
			SI_REFLECTION_MEMBER_AS_TYPE(m_name, uint32_t))
	};

	class RenderMaterial
	{
	public:
		RenderMaterial(){}
		virtual ~RenderMaterial(){}
		
		RendererDrawStageType        GetStage()         const{ return m_stage; }
		GfxVertexShader&             GetVertexShader()       { return m_vertexShader; }
		const GfxVertexShader&       GetVertexShader()  const{ return m_vertexShader; }
		GfxPixelShader&              GetPixelShader()        { return m_pixelShader; }
		const GfxPixelShader&        GetPixelShader()   const{ return m_pixelShader; }
		GfxRootSignatureEx&          GetRootSignature()      { return m_rootSignature; }
		const GfxRootSignatureEx&    GetRootSignature() const{ return m_rootSignature; }
		GfxBufferEx_Constant&        GetConstantBuffer(uint32_t frameIndex)     { return m_constantBuffers[frameIndex]; }
		const GfxBufferEx_Constant&  GetConstantBuffer(uint32_t frameIndex)const{ return m_constantBuffers[frameIndex]; }
		GfxDescriptorHeap&           GetSrvHeap(uint32_t frameIndex)     { return m_srvHeaps[frameIndex]; }
		const GfxDescriptorHeap&     GetSrvHeap(uint32_t frameIndex)const{ return m_srvHeaps[frameIndex]; }
		GfxDescriptorHeap&           GetCbvHeap(uint32_t frameIndex)     { return m_cbvHeaps[frameIndex]; }
		const GfxDescriptorHeap&     GetCbvHeap(uint32_t frameIndex)const{ return m_cbvHeaps[frameIndex]; }
		GfxDescriptorHeap&           GetSamplerHeap(uint32_t frameIndex)     { return m_samplerHeaps[frameIndex]; }
		const GfxDescriptorHeap&     GetSamplerHeap(uint32_t frameIndex)const{ return m_samplerHeaps[frameIndex]; }

	protected:
		RendererDrawStageType  m_stage;
		GfxVertexShader        m_vertexShader;
		GfxPixelShader         m_pixelShader;
		GfxRootSignatureEx     m_rootSignature;

		GfxBufferEx_Constant   m_constantBuffers[kFrameCount];
		GfxDescriptorHeap      m_srvHeaps[kFrameCount];
		GfxDescriptorHeap      m_cbvHeaps[kFrameCount];
		GfxDescriptorHeap      m_samplerHeaps[kFrameCount];
	};	

	class RenderMaterial_Simple : public RenderMaterial
	{
	public:
		RenderMaterial_Simple() {}
		virtual ~RenderMaterial_Simple(){}

		void Initialize();
	};

	class Material
	{
	public:
		Material();
		~Material();
		
		void ExportSerializeData(MaterialSerializeData& outData) const
		{
			outData.m_name = m_name;
		}
		
		void ImportSerializeData(const MaterialSerializeData& serializeData);

		const RenderMaterial* GetRenderMaterialSet(RendererDrawStageType stage) const
		{
			if(!m_mask.CheckEnable(stage)) return nullptr;

			uint32_t count = m_renderMaterials.GetItemCount();
			for(uint32_t i=0; i<count; ++i)
			{
				const RenderMaterial* m = m_renderMaterials.GetItem(i);
				if(m->GetStage()==stage)
				{
					return m;
				}				
			}

			SI_ASSERT(0);
			return nullptr;
		}

	public:
		static Material* Create();
		static void Release(Material*& material);		

	private:
		friend class FbxParser;

	private:
		LongObjectIndex           m_name;
		Array<RenderMaterial*>    m_renderMaterials;
		RendererDrawStageMask     m_mask;
	};

} // namespace SI
