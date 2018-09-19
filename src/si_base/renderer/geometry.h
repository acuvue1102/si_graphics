#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/renderer_common.h"
#include "si_base/gpu/gfx_buffer_ex.h"

namespace SI
{
	struct VertexSemanticsAndFormat
	{
		GfxSemantics   m_semantics;
		GfxFormat      m_format;
		
		VertexSemanticsAndFormat()
			: m_semantics(GfxSemanticsType::Invalid, 0)
			, m_format(GfxFormat::Unknown)
		{
		}
	};

	struct VertexLayout
	{
		uint16_t                   m_stride;
		VertexSemanticsAndFormat   m_attributes[15];
		uint8_t                    m_attributeCount;

		VertexLayout()
			: m_stride(0)
			, m_attributeCount(0)
		{
		}
	};

	class Geometry
	{
	public:
		Geometry()
			: m_vertexLayout()
			, m_is16bitIndex(0)
			, m_vertexBuffer()
			, m_indexBuffer()
		{
		}
		
		virtual ~Geometry()
		{
		}

		virtual uint32_t GetType() const
		{
			return 'Geom';
		}

	public:
		static Geometry* Create();
		static void Release(Geometry*& geometry);

	private:
		friend class FbxParser;

	private:
		VertexLayout m_vertexLayout;
		uint8_t      m_is16bitIndex;
		GfxBufferEx  m_vertexBuffer;
		GfxBufferEx  m_indexBuffer;
	};

	class VerboseGeometry : public Geometry
	{
	public:
		VerboseGeometry()
		{
		}

		virtual ~VerboseGeometry()
		{
			m_rawVertexBuffer.Reset();
			m_rawIndexBuffer.Reset();
		}

		virtual uint32_t GetType() const
		{
			return 'Verb';
		}

	private:
		static VerboseGeometry* Create();

	private:
		friend class FbxParser;

	private:
		Array<uint8_t>   m_rawVertexBuffer;
		Array<uint8_t>   m_rawIndexBuffer;

		SI_REFLECTION(
			SI::VerboseGeometry,
			SI_REFLECTION_MEMBER(m_rawVertexBuffer),
			SI_REFLECTION_MEMBER(m_rawIndexBuffer))
	};

} // namespace SI
