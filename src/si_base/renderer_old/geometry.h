#pragma once

#include "si_base/container/array.h"
#include "si_base/renderer/renderer_common.h"
#include "si_base/gpu/gfx_buffer_ex.h"
#include "si_base/gpu/gfx_input_layout.h"

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

	static const size_t kMaxVertexAttributeCount = 15;
	struct VertexLayout
	{
		uint16_t                   m_stride;
		uint8_t                    m_attributeCount;
		VertexSemanticsAndFormat   m_attributes[kMaxVertexAttributeCount];

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
		
		virtual ~Geometry();

		virtual uint32_t GetType() const
		{
			return 'Geom';
		}

		virtual void ExportSerializeData(GeometrySerializeData& outData) const
		{
			SI_ASSERT(0);
		}

		virtual void ImportSerializeData(const GeometrySerializeData& s);

		
		const VertexLayout& GetVertexLayout() const{ return m_vertexLayout; }
		bool Is16BitIndex() const{ return m_is16bitIndex; }
		const GfxBufferEx_Vertex& GetVertexBuffer() const{ return m_vertexBuffer; }
		const GfxBufferEx_Index& GetIndexBuffer() const{ return m_indexBuffer; }
		uint32_t GetIndexCount() const{ return (uint32_t)(m_indexBuffer.GetSize()/(m_is16bitIndex? 2 :4)); }

		const GfxInputElement* GetInputElements() const{ return m_inputElements.GetItemsAddr(); }
		uint32_t GetInputElementCount() const{ return m_inputElements.GetItemCount(); }

	public:
		static Geometry* Create();
		static void Release(Geometry*& geometry);

	private:
		friend class FbxParser;

	protected:
		VertexLayout               m_vertexLayout;
		uint8_t                    m_is16bitIndex;
		GfxBufferEx_Vertex         m_vertexBuffer;
		GfxBufferEx_Index          m_indexBuffer;
		SafeArray<GfxInputElement> m_inputElements;
	};

} // namespace SI
