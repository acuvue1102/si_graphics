
#include "si_base/renderer/renderer.h"

#include "si_base/math/vfloat4x4.h"

namespace SI
{
	struct SceneCB
	{
		Vfloat4x4 m_view;
		Vfloat4x4 m_proj;
		Vfloat4x4 m_viewProj;
	};

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
		// コンスタントバッファのセットアップ
		for(uint32_t i=0; i<kFrameCount; ++i)
		{
			m_sceneCB[i].InitializeAsConstant("SceneCB", sizeof(SceneCB));
			SceneCB* cb = static_cast<SceneCB*>(m_sceneCB[i].GetMapPtr());
			memset(cb, 0, sizeof(SceneCB));

			cb->m_view      = Vfloat4x4::Identity();
			cb->m_proj      = Vfloat4x4::Identity();
			cb->m_viewProj  = Vfloat4x4::Identity();
		}

		m_dummyCB.InitializeAsConstant("DummyCB", sizeof(float)*4);
		float* dummy = static_cast<float*>(m_dummyCB.GetMapPtr());
		dummy[0] = dummy[1] = dummy[2] = dummy[3] = 0.0f;
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
		m_frameIndex = GetWriteFrameIndex(); // flip

	}

	void Renderer::Render()
	{
	}
	
} // namespace SI
