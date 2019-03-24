#pragma once

#include "si_base/container/array.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_buffer_ex.h"
#include "si_base/gpu/gfx_root_signature_ex.h"
#include "si_base/gpu/gfx_texture_ex.h"
#include "si_base/gpu/gfx_descriptor_heap_ex.h"
#include "si_base/renderer/renderer_common.h"
#include "si_base/renderer/renderer_draw_stage_type.h"
#include "si_base/renderer/material_type.h"

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

	class RenderMaterial : private NonCopyable
	{
	public:
		RenderMaterial() : m_CBVRootIndexOffset(0){}
		virtual ~RenderMaterial(){}
		
		virtual RendererDrawStageType GetStageType()     const = 0;

		GfxVertexShader&             GetVertexShader()       { return m_vertexShader; }
		const GfxVertexShader&       GetVertexShader()  const{ return m_vertexShader; }
		GfxPixelShader&              GetPixelShader()        { return m_pixelShader; }
		const GfxPixelShader&        GetPixelShader()   const{ return m_pixelShader; }
		GfxRootSignatureEx&          GetRootSignature()      { return m_rootSignature; }
		const GfxRootSignatureEx&    GetRootSignature() const{ return m_rootSignature; }
		GfxBufferEx_Constant&        GetConstantBuffer(uint32_t frameIndex)     { return m_constantBuffers[frameIndex]; }
		const GfxBufferEx_Constant&  GetConstantBuffer(uint32_t frameIndex)const{ return m_constantBuffers[frameIndex]; }
		GfxDescriptorHeapEx&         GetSrvHeap(uint32_t frameIndex)     { return m_srvHeaps[frameIndex]; }
		const GfxDescriptorHeapEx&   GetSrvHeap(uint32_t frameIndex)const{ return m_srvHeaps[frameIndex]; }
		GfxDescriptorHeapEx&         GetSamplerHeap(uint32_t frameIndex)     { return m_samplerHeaps[frameIndex]; }
		const GfxDescriptorHeapEx&   GetSamplerHeap(uint32_t frameIndex)const{ return m_samplerHeaps[frameIndex]; }

	protected:
		static const uint32_t kReserveCbv = 2; // SceneDB + InstanceDB
		
		bool DefaultSetup(
			const char* name,
			const char* shaderPath,
			int8_t materialConstantSlot=2);

		void SetupDefaultRootSignatureDescEx(
			GfxRootSignatureDescEx& outRootSignatureDesc,
			const char* name,
			uint32_t srvCount = 0,
			uint32_t samplerCount = 0,
			uint32_t cbvCount = kReserveCbv+1);

	protected:
		GfxVertexShader        m_vertexShader;
		GfxPixelShader         m_pixelShader;
		GfxRootSignatureEx     m_rootSignature;

		GfxBufferEx_Constant   m_constantBuffers[kFrameCount];
		GfxDescriptorHeapEx    m_srvHeaps[kFrameCount];
		GfxDescriptorHeapEx    m_samplerHeaps[kFrameCount];
		uint32_t               m_CBVRootIndexOffset;
	};	

	class Material : private NonCopyable
	{
	public:
		Material();
		virtual ~Material();
		
		virtual void ExportSerializeData(MaterialSerializeData& outData) const = 0;		
		virtual void ImportSerializeData(const MaterialSerializeData& serializeData) = 0;

		RenderMaterial* GetRenderMaterialSet(RendererDrawStageType stageType)
		{
			if(!m_mask.CheckEnable(stageType)) return nullptr;

			uint32_t count = m_renderMaterials.GetItemCount();
			for(uint32_t i=0; i<count; ++i)
			{
				RenderMaterial* m = m_renderMaterials.GetItem(i);
				if(m->GetStageType()==stageType)
				{
					return m;
				}				
			}

			SI_ASSERT(0);
			return nullptr;
		}

		const RenderMaterial* GetRenderMaterialSet(RendererDrawStageType stageType) const
		{
			return GetRenderMaterialSet(stageType);
		}
		
		RendererDrawStageMask GetDrawStageMask() const
		{
			return m_mask;
		}

		void SetupDrawStageMask()
		{
			m_mask.Reset();
			uint32_t count = m_renderMaterials.GetItemCount();
			for(uint32_t i=0; i<count; ++i)
			{
				RenderMaterial* m = m_renderMaterials.GetItem(i);
				RendererDrawStageType stageType = m->GetStageType();

				SI_ASSERT(!m_mask.CheckEnable(stageType), "同じステージに2個以上RenderMaterialがある.");
				m_mask.EnableMaskBit(stageType);
			}
		}

		virtual void UpdateRenderMaterial(RenderMaterial& renderMaterial, uint32_t frameIndex) const = 0;

	protected:
		friend class FbxParser;

	protected:
		LongObjectIndex            m_name;
		SafeArray<RenderMaterial*> m_renderMaterials;
		RendererDrawStageMask      m_mask;
	};

} // namespace SI
