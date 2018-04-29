#pragma once

#include <stdint.h>

namespace SI
{
	struct PipelineDesc
	{
		uint32_t m_width  = 1280;
		uint32_t m_height = 720;
		void*   m_hWnd    = nullptr;
	};

	class PipelineImpl;

	class Pipeline
	{
	public:
		Pipeline();
		~Pipeline();

		int Initialize(const PipelineDesc& desc);
		int Terminate();

		int Render();

	private:
		PipelineImpl* m_impl;
	};

} // namespace SI
