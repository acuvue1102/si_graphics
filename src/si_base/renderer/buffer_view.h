#pragma once

#include <memory>
#include <vector>
#include "si_base/core/assert.h"

namespace SI
{
	class BufferView
	{
	public:
		BufferView()
			: m_bufferId(-1)
			, m_sizeInByte(0)
			, m_strideInByte(0)
			, m_offsetInByte(0)
		{}

		BufferView(
			int bufferId,
			size_t sizeInByte,
			size_t strideInByte,
			size_t offsetInByte)
			: m_bufferId(bufferId)
			, m_sizeInByte(sizeInByte)
			, m_strideInByte(strideInByte)
			, m_offsetInByte(offsetInByte)
		{}

		inline void             SetBufferId(int  bufferId) { m_bufferId = bufferId;             }
		inline void             SetSize  (size_t sizeInByte) { m_sizeInByte = sizeInByte; }
		inline void             SetStride(size_t strideInByte) { m_strideInByte = strideInByte; }
		inline void             SetOffset(size_t offsetInByte) { m_offsetInByte = offsetInByte; }

		inline int              GetBufferId() const{ return m_bufferId; }
		inline size_t           GetSize()   const{ return m_sizeInByte; }
		inline size_t           GetStride() const{ return m_strideInByte; }
		inline size_t           GetOffset() const{ return m_offsetInByte; }

	private:
		int        m_bufferId;
		size_t     m_sizeInByte;
		size_t     m_strideInByte;
		size_t     m_offsetInByte;
	};

} // namespace SI
