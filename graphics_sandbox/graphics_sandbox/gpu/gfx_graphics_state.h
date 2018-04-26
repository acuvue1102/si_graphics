#pragma once

namespace SI
{
	class BaseGraphicsState;

	class GfxGraphicsState
	{
	public:
		GfxGraphicsState(BaseGraphicsState* base=nullptr);
		~GfxGraphicsState();

	public:
		BaseGraphicsState* GetBaseGraphicsState(){ return m_base; }
		const BaseGraphicsState* GetBaseGraphicsState() const{ return m_base; }

	private:
		BaseGraphicsState* m_base;
	};

} // namespace SI
