
#include "si_base/renderer/geometry.h"

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
		
		GfxBuffer vb = m_vertexBuffer.GetBuffer();
		device.UploadBufferLater(vb, s.m_rawVertexBuffer.GetItemsAddr(), vb.GetSize());

		m_indexBuffer.InitializeAsIndex(
			"index",
			s.m_rawIndexBuffer.GetItemCount() / (m_is16bitIndex? 2 : 4),
			m_is16bitIndex);
		GfxBuffer ib = m_indexBuffer.GetBuffer();
		device.UploadBufferLater(ib, s.m_rawIndexBuffer.GetItemsAddr(), ib.GetSize());
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
