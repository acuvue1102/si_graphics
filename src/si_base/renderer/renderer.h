#pragma once

#include <unordered_map>
#include "si_base/renderer/renderer_common.h"
#include "si_base/container/array.h"
#include "si_base/core/singleton.h"
#include "si_base/renderer/model_instance.h"
#include "si_base/gpu/gfx_linear_allocator.h"

namespace SI
{
	struct RenderDesc;
	class GfxGraphicsContext;

	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer();
		~Renderer();

		void Initialize();
		void Terminate();
		
		void Add(ModelInstancePtr& modelInstance);
		void Remove(ModelInstancePtr& modelInstance);
		
		void SetViewMatrix(Vfloat4x4_arg view){ m_viewMatrix = view; }
		void SetProjectionMatrix(Vfloat4x4_arg proj){ m_projectionMatrix = proj; }

		void Update();
		void Render(
			GfxGraphicsContext& context,
			RendererDrawStageType stageType,
			const RendererGraphicsStateDesc& desc);
		
		uint32_t GetReadFrameIndex()  const{ return (uint32_t)(m_frameIndex%(uint64_t)kFrameCount); }
		uint32_t GetWriteFrameIndex() const{ return (uint32_t)((m_frameIndex+1)%(uint64_t)kFrameCount); }
		
		//GfxBufferEx_Constant& GetSceneDB(uint32_t frameIndex){ return m_sceneCB[frameIndex]; }
		//GfxBufferEx_Constant& GetDummyCB(){ return m_dummyCB; }

	private:
		uint64_t               m_frameIndex;
		//GfxBufferEx_Constant   m_sceneCB[kFrameCount];
		//GfxBufferEx_Constant   m_dummyCB;
		GfxLinearAllocator     m_constantAllocator;

		std::unordered_map<void*, ModelInstancePtr> m_models;
		Vfloat4x4 m_viewMatrix;
		Vfloat4x4 m_projectionMatrix;
	};

} // namespace SI
