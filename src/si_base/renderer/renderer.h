#pragma once

#include "si_base/renderer/renderer_common.h"
#include "si_base/container/array.h"
#include "si_base/core/singleton.h"
#include "si_base/renderer/model_instance.h"

namespace SI
{
	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer();
		~Renderer();

		void Initialize();
		
		void Add(ModelInstancePtr& modelInstance);
		void Remove(ModelInstancePtr& modelInstance);

		void Update();
		void Render();
		
		uint32_t GetReadFrameIndex()  const{ return m_frameIndex; }
		uint32_t GetWriteFrameIndex() const{ return (m_frameIndex+1)%kFrameCount; }
		
		GfxBufferEx_Constant& GetSceneDB(uint32_t frameIndex){ return m_sceneCB[frameIndex]; }
		GfxBufferEx_Constant& GetDummyCB(){ return m_dummyCB; }

	private:
		uint32_t               m_frameIndex;
		GfxBufferEx_Constant   m_sceneCB[kFrameCount];
		GfxBufferEx_Constant   m_dummyCB;

		std::unordered_map<void*, ModelInstancePtr> m_models;
	};

} // namespace SI
