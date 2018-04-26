#pragma once

#include <stdint.h>
#include "gpu/gfx.h"

namespace SI
{
	class Pipeline;

	struct AppConfig
	{
		uint32_t m_width  = 1280;
		uint32_t m_height = 720;
		int    m_nCmdShow = 0;
	};

	class App
	{
	public:
		App();
		~App();

		int Initialize(const AppConfig& config);
		int Terminate();

		int Run();

	private:
		Pipeline*         m_pipeline;

		bool              m_initialized;
	};

} // namespace SI