//
// app.h
///
// appƒNƒ‰ƒX. 
// observer‚ğ“o˜^‚µ‚ÄARun‚ğŒÄ‚Ô
//
#pragma once

#include <stdint.h>
#include <vector>

namespace SI
{
	class PathStorage;
	class AppObserver;

	struct AppDesc
	{
		uint32_t       m_width         = 1280;
		uint32_t       m_height        = 720;
		int            m_nCmdShow      = 0;
		uint32_t       m_observerCount = 0;
		AppObserver**  m_observers     = nullptr;
	};

	class App
	{
	public:
		App();
		~App();

		int Run(const AppDesc& desc);

	private:
		int Initialize(const AppDesc& desc);
		int Terminate();
		void Update();
		void Render();

	private:
		PathStorage*                m_pathStorage;
		std::vector<AppObserver*>   m_objservers;
		bool                        m_initialized;
	};

} // namespace SI