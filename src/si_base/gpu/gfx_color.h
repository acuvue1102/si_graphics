#pragma once

namespace SI
{
	class GfxColorRGBA
	{
	public:
		GfxColorRGBA()
			: m_color{0.0f, 0.0f, 0.0f, 0.0f}
		{
		}

		GfxColorRGBA(float r, float g, float b, float a)
			: m_color{r, g, b, a}
		{
		}

		explicit GfxColorRGBA(float c)
			: m_color{c, c, c, c}
		{
		}

		explicit GfxColorRGBA(const float* color)
			: m_color{color[0], color[1], color[2], color[3]}
		{
		}

		~GfxColorRGBA()
		{
		}
		
	public:
		void Set(float r, float g, float b, float a)
		{
			m_color[0] = r;
			m_color[1] = g;
			m_color[2] = b;
			m_color[3] = a;
		}		
		void Set(const float* color)
		{
			m_color[0] = color[0];
			m_color[1] = color[1];
			m_color[2] = color[2];
			m_color[3] = color[3];
		}		
		void SetR(float r){ m_color[0] = r; }
		void SetG(float g){ m_color[1] = g; }
		void SetB(float b){ m_color[2] = b; }
		void SetA(float a){ m_color[3] = a; }
		
		void Get(float* color) const
		{
			color[0] = m_color[0];
			color[1] = m_color[1];
			color[2] = m_color[2];
			color[3] = m_color[3];
		}
		float R() const{ return m_color[0]; }
		float G() const{ return m_color[1]; }
		float B() const{ return m_color[2]; }
		float A() const{ return m_color[3]; }
		
		const float* GetPtr() const
		{
			return m_color;
		}

	public:
		float& operator[](size_t i)
		{
			return m_color[i];
		}

		const float& operator[](size_t i) const
		{
			return m_color[i];
		}

	private:
		float m_color[4];
	};

	class GfxDepthStencil
	{
	public:
		GfxDepthStencil()
			: m_depth(1.0f)
			, m_stencil(0)
		{
		}

		GfxDepthStencil(float depth, uint8_t stencil)
			: m_depth(depth)
			, m_stencil(stencil)
		{
		}

		void SetDepth(float depth)
		{
			m_depth = depth;
		}

		void SetStencil(uint8_t stencil)
		{
			m_stencil = stencil;
		}

		float GetDepth() const
		{
			return m_depth;
		}

		uint8_t GetStencil() const
		{
			return m_stencil;
		}

	private:
		float    m_depth;
		uint8_t  m_stencil;
	};

} // namespace SI
