#pragma once

#include <cstdint>
#include <si_app/app/app.h>

namespace SI
{
	class TrackballCamera;

namespace APP004
{
	class Pipeline;

	class App004  : public App
	{
		using Super = App;
	public:
		App004();
		virtual ~App004();
		
		virtual void OnPreUpdate   (const AppUpdateInfo& updateInfo) override;

	private:
		Pipeline*         m_pipeline;
		TrackballCamera*  m_camera;
	};
	
} // namespace APP004
} // namespace SI
