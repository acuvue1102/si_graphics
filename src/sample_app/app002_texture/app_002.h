#pragma once

#include <cstdint>
#include <si_app/app/app.h>

namespace SI
{
	class TrackballCamera;

namespace APP002
{
	class Pipeline;

	class App002  : public App
	{
		using Super = App;
	public:
		App002();
		virtual ~App002();
		
		virtual void OnPreUpdate   (const AppUpdateInfo& updateInfo) override;

	private:
		Pipeline*         m_pipeline;
		TrackballCamera*  m_camera;
	};
	
} // namespace APP002
} // namespace SI
