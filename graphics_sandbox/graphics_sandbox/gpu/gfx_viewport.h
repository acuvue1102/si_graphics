#pragma once

namespace SI
{
	class GfxViewport
	{
	public:
		GfxViewport(
			float topLeftX = 0.0f,
			float topLeftY = 0.0f,
			float width    = 1920.0f,
			float height   = 1080.0f,
			float minDepth = 0.0f,
			float maxDepth = 1.0f)
			: m_topLeftX ( topLeftX )
			, m_topLeftY ( topLeftY )
			, m_width    ( width )
			, m_height   ( height )
			, m_minDepth ( minDepth )
			, m_maxDepth ( maxDepth )
		{
		}

		~GfxViewport(){}
		
		inline void Setup(
			float topLeftX,
			float topLeftY,
			float width,
			float height,
			float minDepth,
			float maxDepth)
		{
			m_topLeftX = topLeftX;
			m_topLeftY = topLeftY;
			m_width    = width;
			m_height   = height;
			m_minDepth = minDepth;
			m_maxDepth = maxDepth;
		}
		
		inline float GetTopLeftX() const{ return m_topLeftX; }
		inline float GetTopLeftY() const{ return m_topLeftY; }
		inline float GetWidth()    const{ return m_width; }
		inline float GetHeight()   const{ return m_height; }
		inline float GetMinDepth() const{ return m_minDepth; }
		inline float GetMaxDepth() const{ return m_maxDepth; }

	private:
		float m_topLeftX;
		float m_topLeftY;
		float m_width;
		float m_height;
		float m_minDepth;
		float m_maxDepth;
	};

	///////////////////////////////////////////////////////////////

	class GfxScissor
	{
	public:
		GfxScissor(
			int left   = 0,
			int top    = 0,
			int right  = 1920,
			int bottom = 1080)
			: m_left(left)
			, m_top(top)
			, m_right(right)
			, m_bottom(bottom)
		{
		}

		~GfxScissor()
		{
		}

		void Setup(
			int left,
			int top,
			int right,
			int bottom)
		{
			m_left   = left;
			m_top    = top;
			m_right  = right;
			m_bottom = bottom;
		}
		
		int GetLeft()   const{ return m_left; }
		int GetTop()    const{ return m_top; }
		int GetRight()  const{ return m_right; }
		int GetBottom() const{ return m_bottom; }

	private:
		int m_left;
		int m_top;
		int m_right;
		int m_bottom;
	};

} // namespace SI
