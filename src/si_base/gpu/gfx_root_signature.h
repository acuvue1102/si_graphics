#pragma once

namespace SI
{
	class BaseRootSignature;

	class GfxRootSignature
	{
	public:
		GfxRootSignature(BaseRootSignature* base=nullptr)
			: m_base(base)
		{
		}

		~GfxRootSignature()
		{
		}

	public:
		BaseRootSignature*       GetBaseRootSignature()      { return m_base; }
		const BaseRootSignature* GetBaseRootSignature() const{ return m_base; }

	private:
		BaseRootSignature* m_base;
	};
} // namespace SI
