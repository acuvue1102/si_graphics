
#include "si_base/renderer/geometry.h"

#include "si_base/gpu/gfx_utility.h"

namespace SI
{
	Geometry::~Geometry()
	{
		m_indexBuffer.TerminateAsIndex();
		m_vertexBuffer.TerminateAsVertex();
	}

	void Geometry::ImportSerializeData(const GeometrySerializeData& s)
	{
		GfxDevice& device = *GfxDevice::GetInstance();

		m_vertexLayout = s.m_vertexLayout;
		m_is16bitIndex = s.m_is16bitIndex;

		m_vertexBuffer.InitializeAsVertex(
			"vertex",
			s.m_rawVertexBuffer.GetItemCount()/m_vertexLayout.m_stride,
			m_vertexLayout.m_stride,
			0);
		
		GfxBuffer vb = m_vertexBuffer.Get();
		device.UploadBufferLater(
			vb,
			s.m_rawVertexBuffer.GetItemsAddr(),
			vb.GetSize(),
			GfxResourceState::CopyDest,
			GfxResourceState::VertexAndConstantBuffer);

		m_indexBuffer.InitializeAsIndex(
			"index",
			s.m_rawIndexBuffer.GetItemCount() / (m_is16bitIndex!=0? 2 : 4),
			m_is16bitIndex!=0);
		GfxBuffer ib = m_indexBuffer.Get();
		device.UploadBufferLater(
			ib,
			s.m_rawIndexBuffer.GetItemsAddr(),
			ib.GetSize(),
			GfxResourceState::CopyDest,
			GfxResourceState::IndexBuffer);
				
		m_inputElements.Setup(m_vertexLayout.m_attributeCount);
		uint32_t offset = 0;
		for(uint8_t attr=0; attr<m_vertexLayout.m_attributeCount; ++attr)
		{
			const VertexSemanticsAndFormat& vsf = m_vertexLayout.m_attributes[attr];
			GfxInputElement& ve = m_inputElements[attr];
			ve = GfxInputElement(
				GetSemanticsName(vsf.m_semantics.m_semanticsType),
				vsf.m_semantics.m_semanticsIndex,
				vsf.m_format,
				0,
				offset);

			uint32_t formatBits = (uint32_t)GetFormatBits(vsf.m_format);
			SI_ASSERT(formatBits%8==0);
				
			offset += formatBits/8;
		}
	}

	Geometry* Geometry::Create()
	{
		return new Geometry();
	}

	void Geometry::Release(Geometry*& geometry)
	{
		delete geometry;
		geometry = nullptr;
	}
	

} // namespace SI
