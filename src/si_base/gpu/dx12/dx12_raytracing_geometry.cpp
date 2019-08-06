
#include "si_base/gpu/dx12/dx12_raytracing_geometry.h"
#include "si_base/gpu/dx12/dx12_device.h"
#include "si_base/gpu/gfx_buffer.h"


namespace SI
{
	BaseRaytracingScene::BaseRaytracingScene()
		: m_bottomLevelAccelerationStructure(nullptr)
		, m_topLevelAccelerationStructure(nullptr)
	{
	}

	BaseRaytracingScene::~BaseRaytracingScene()
	{
		SI_ASSERT(m_bottomLevelAccelerationStructure == nullptr);
		SI_ASSERT(m_topLevelAccelerationStructure == nullptr);
	}

	void BaseRaytracingScene::Initialize(BaseDevice* baseDevice, size_t bottomASSize, size_t topASSize)
	{
		SI_ASSERT(m_bottomLevelAccelerationStructure == nullptr);
		SI_ASSERT(m_topLevelAccelerationStructure == nullptr);

		GfxBufferDesc bottomASDesc;
		bottomASDesc.m_name = "BottomLevelAccelerationStructure";
		bottomASDesc.m_bufferSizeInByte = bottomASSize;
		bottomASDesc.m_resourceStates = GfxResourceState::RaytracingAccelerationStructure;
		bottomASDesc.m_resourceFlags = GfxResourceFlag::AllowUnorderedAccess;
		m_bottomLevelAccelerationStructure = baseDevice->CreateBuffer(bottomASDesc);

		GfxBufferDesc topASDesc;
		topASDesc.m_name = "TopLevelAccelerationStructure";
		topASDesc.m_bufferSizeInByte = topASSize;
		topASDesc.m_resourceStates = GfxResourceState::RaytracingAccelerationStructure;
		topASDesc.m_resourceFlags = GfxResourceFlag::AllowUnorderedAccess;
		m_topLevelAccelerationStructure = baseDevice->CreateBuffer(topASDesc);
	}

	void BaseRaytracingScene::Release(BaseDevice* baseDevice)
	{
		if(m_bottomLevelAccelerationStructure)
		{
			baseDevice->ReleaseBuffer(m_bottomLevelAccelerationStructure);
			m_bottomLevelAccelerationStructure = nullptr;
		}

		if(m_topLevelAccelerationStructure)
		{
			baseDevice->ReleaseBuffer(m_topLevelAccelerationStructure);
			m_topLevelAccelerationStructure = nullptr;
		}
	}

} // namespace SI
