#pragma once

namespace SI
{
	class BaseFenceEvent;
	class BaseFence;

	class GfxFenceEvent
	{
	public:
		GfxFenceEvent(BaseFenceEvent* base=nullptr);
		~GfxFenceEvent();

	public:
		BaseFenceEvent* GetBaseFenceEvent(){ return m_base; }
		const BaseFenceEvent* GetBaseFenceEvent() const{ return m_base; }

	public:
		BaseFenceEvent* m_base;
	};


	class GfxFence
	{
	public:
		GfxFence(BaseFence* base=nullptr);
		~GfxFence();

	public:
		

	public:
		BaseFence* GetBaseFence(){ return m_base; }
		const BaseFence* GetBaseFence() const{ return m_base; }

	private:
		BaseFence* m_base;
	};

} // namespace SI