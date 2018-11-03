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

		SI_REFLECTION(
			SI::VertexSemanticsAndFormat,
			SI_REFLECTION_MEMBER_AS_TYPE(m_semantics, uint8_t),
			SI_REFLECTION_MEMBER_AS_TYPE(m_format, uint8_t))
	};

	struct VertexLayout
	{
		uint16_t                   m_stride;
		uint8_t                    m_attributeCount;
		VertexSemanticsAndFormat   m_attributes[15];

		VertexLayout()
			: m_stride(0)
			, m_attributeCount(0)
		{
		}

		SI_REFLECTION(
			SI::VertexLayout,
			SI_REFLECTION_MEMBER(m_stride),
			SI_REFLECTION_MEMBER(m_attributeCount),
			SI_REFLECTION_MEMBER_ARRAY(m_attributes))
	};

	struct GeometrySerializeData
	{
		VertexLayout     m_vertexLayout;
		bool             m_is16bitIndex;
		Array<uint8_t>   m_rawVertexBuffer;
		Array<uint8_t>   m_rawIndexBuffer;

		GeometrySerializeData()
			: m_is16bitIndex(false)
		{
		}

		SI_REFLECTION(
			SI::GeometrySerializeData,
			SI_REFLECTION_MEMBER(m_vertexLayout),
			SI_REFLECTION_MEMBER(m_is16bitIndex),
			SI_REFLECTION_MEMBER(m_rawVertexBuffer),
			SI_REFLECTION_MEMBER(m_rawIndexBuffer))
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

		virtual void ExportSerializeData(GeometrySerializeData& outData) const
		{
			SI_ASSERT(0);
		}

		virtual void ImportSerializeData(const GeometrySerializeData& s)
		{
			m_vertexLayout = s.m_vertexLayout;
			m_is16bitIndex = s.m_is16bitIndex;

			// TODO : setup
		}

	public:
		static Geometry* Create();
		static void Release(Geometry*& geometry);

	private:
		friend class FbxParser;

	protected:
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

		virtual void ExportSerializeData(GeometrySerializeData& outData) const override
		{
			outData.m_vertexLayout    = m_vertexLayout;
			outData.m_is16bitIndex    = m_is16bitIndex;
			outData.m_rawVertexBuffer = m_rawVertexBuffer;
			outData.m_rawIndexBuffer  = m_rawIndexBuffer;
		}

		virtual void ImportSerializeData(const GeometrySerializeData& s) override
		{
			SI_ASSERT(0);
			return;
		}

	private:
		static VerboseGeometry* Create();

	private:
		friend class FbxParser;

	protected:
		Array<uint8_t>   m_rawVertexBuffer;
		Array<uint8_t>   m_rawIndexBuffer;
	};

} // namespace SI
