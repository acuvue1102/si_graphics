#pragma once

#include <memory>
#include <vector>
#include "si_base/core/assert.h"
#include "si_base/misc/string.h"
#include "si_base/math/math.h"

#include "si_base/renderer/renderer_common.h"
#include "si_base/renderer/renderer_draw_stage_type.h"
#include "si_base/gpu/gfx.h"

namespace SI
{
	class IScenes;
	class Material;

	struct FloatAttribute
	{
		const char* m_name;
		const char* m_filePath;
		float m_value;

		FloatAttribute()
			: m_name(nullptr)
			, m_filePath(nullptr)
			, m_value(0.0f)
		{
		}
	};

	struct BoolAttribute
	{
		const char* m_name;
		const char* m_filePath;
		bool m_value;

		BoolAttribute()
			: m_name(nullptr)
			, m_filePath(nullptr)
			, m_value(false)
		{
		}
	};

	struct IntAttribute
	{
		const char* m_name;
		const char* m_filePath;
		int m_value;

		IntAttribute()
			: m_name(nullptr)
			, m_filePath(nullptr)
			, m_value(0)
		{
		}
	};

	struct Vfloat4Attribute
	{
		const char* m_name;
		const char* m_filePath;
		Vfloat4 m_value;

		Vfloat4Attribute()
			: m_name(nullptr)
			, m_filePath(nullptr)
			, m_value(0.0f)
		{
		}
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

		virtual void RenderSetup(uint32_t frameIndex, const IScenes& scenes, const Material& material){}

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

		static GfxTexture GetTexture(int textureId, const IScenes& scenes, const GfxTexture& defaultTex);

	protected:
		GfxVertexShader        m_vertexShader;
		GfxPixelShader         m_pixelShader;
		GfxRootSignatureEx     m_rootSignature;

		GfxBufferEx_Constant   m_constantBuffers[kFrameCount];
		GfxDescriptorHeapEx    m_srvHeaps[kFrameCount];
		GfxDescriptorHeapEx    m_samplerHeaps[kFrameCount];
		uint32_t               m_CBVRootIndexOffset;
	};	

	class Material
	{
	public:
		Material()
			: m_baseColorTextureId(-1)
			, m_normalTextureId(-1)
			, m_metallicRoughnessTextureId(-1)
			, m_emissiveTextureId(-1)
			, m_baseColorFactor(1.0f)
			, m_normalFactor(1.0f)
			, m_metallicFactor(1.0f)
			, m_roughnessFactor(1.0f)
			, m_emissiveFactor(0.0f)
		{}
		~Material();

		void Setup();

		void SetName(const char* name){ m_name = name; }
		const char* GetName() const{ return m_name.c_str(); }

		RenderMaterial* GetRenderMaterial(RendererDrawStageType stageType)
		{
			if(!m_mask.CheckEnable(stageType)) return nullptr;

			size_t count = m_renderMaterials.size();
			for(size_t i=0; i<count; ++i)
			{
				RenderMaterial* m = m_renderMaterials[i];
				if(m->GetStageType()==stageType)
				{
					return m;
				}				
			}

			SI_ASSERT(0);
			return nullptr;
		}

		const RenderMaterial* GetRenderMaterial(RendererDrawStageType stageType) const
		{
			return GetRenderMaterial(stageType);
		}

		RendererDrawStageMask GetDrawStageMask() const
		{
			return m_mask;
		}

		void SetBaseColorTextureId(int id) { m_baseColorTextureId = id; }
		void SetNormalTextureId(int id) { m_normalTextureId = id; }
		void SetMetallicRoughnessTextureId(int id) { m_metallicRoughnessTextureId = id; }
		void SetEmissiveTextureId(int id) { m_emissiveTextureId = id; }
		void SetBaseColorFactor(Vfloat4 f) { m_baseColorFactor = f; }
		void SetNormalFactor(float f) { m_normalFactor = f; }
		void SetMetallicFactor(float f) { m_metallicFactor = f; }
		void SetRoughnessFactor(float f) { m_roughnessFactor = f; }
		void SetEmissiveFactor(Vfloat3 f) { m_emissiveFactor = f; }

		int GetBaseColorTextureId() const{ return m_baseColorTextureId; }
		int GetNormalTextureId() const{ m_normalTextureId; }
		int GetMetallicRoughnessTextureId() const{ return m_metallicRoughnessTextureId; }
		int GetEmissiveTextureId() const{ return m_emissiveTextureId; }
		Vfloat4 GetBaseColorFactor() const{ return m_baseColorFactor; }
		float GetNormalFactor() const{ return  m_normalFactor; }
		float GetMetallicFactor() const{ return  m_metallicFactor; }
		float GetRoughnessFactor() const{ return  m_roughnessFactor; }
		Vfloat3 GetEmissiveFactor() const{ return  m_emissiveFactor; }

		void SetupDrawStageMask();

		void UpdateRenderMaterial(uint32_t frameIndex, const IScenes& scenes, RenderMaterial* renderMaterial) const;

	private:
		String                       m_name;
		int                          m_baseColorTextureId;
		int                          m_normalTextureId;
		int                          m_metallicRoughnessTextureId;
		int                          m_emissiveTextureId;

		Vfloat4                      m_baseColorFactor;
		float                        m_normalFactor;
		float                        m_metallicFactor;
		float                        m_roughnessFactor;
		Vfloat3                      m_emissiveFactor;

		std::vector<RenderMaterial*> m_renderMaterials;
		RendererDrawStageMask        m_mask;
	};

} // namespace SI
