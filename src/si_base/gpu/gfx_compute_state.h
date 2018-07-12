#pragma once

#include "si_base/gpu/gfx_root_signature.h"
#include "si_base/gpu/gfx_shader.h"
#include "si_base/gpu/gfx_enum.h"

namespace SI
{
	class BaseComputeState;
	class GfxShader;
	class GfxRootSignature;
	struct GfxInputElement;

	struct GfxComputeStateDesc
	{
		const char*              m_name                   = nullptr;
		GfxRootSignature*        m_rootSignature          = nullptr;
		GfxComputeShader*        m_computeShader          = nullptr;
	};

	class GfxComputeState
	{
	public:
		GfxComputeState(BaseComputeState* base=nullptr);
		~GfxComputeState();

		int Release();

	public:
		BaseComputeState* GetBaseComputeState(){ return m_base; }
		const BaseComputeState* GetBaseComputeState() const{ return m_base; }

	private:
		BaseComputeState* m_base;
	};

} // namespace SI
