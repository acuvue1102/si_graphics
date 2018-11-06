
#include "si_base/renderer/geometry.h"

namespace SI
{
	void Geometry::ImportSerializeData(const GeometrySerializeData& s)
	{
		m_vertexLayout = s.m_vertexLayout;
		m_is16bitIndex = s.m_is16bitIndex;

		m_vertexBuffer.InitializeAsVertex(
			"vertex",
			s.m_rawVertexBuffer.GetItemCount(),
			m_vertexLayout.m_stride,
			0);
		GfxBuffer vb = m_vertexBuffer.GetBuffer();
		void* vbuffer = vb.Map();
		if(vbuffer)
		{
			memcpy(vbuffer, s.m_rawVertexBuffer.GetItemsAddr(), vb.GetSize());
			vb.Unmap();
		}

		m_indexBuffer.InitializeAsIndex(
			"index",
			s.m_rawIndexBuffer.GetItemCount(),
			m_is16bitIndex);
		GfxBuffer ib = m_vertexBuffer.GetBuffer();
		void* ibuffer = ib.Map();
		if(ibuffer)
		{
			memcpy(ibuffer, s.m_rawIndexBuffer.GetItemsAddr(), ib.GetSize());
			ib.Unmap();
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
	
	VerboseGeometry* VerboseGeometry::Create()
	{
		return new VerboseGeometry();
	}

} // namespace SI
