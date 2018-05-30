
#include "app_002.h"

#include "pipeline_002.h"
#include "si_app/camera/trackball_camera.h"

namespace SI
{
namespace APP002
{
	//////////////////////////////////////////////////////////
	
	App002::App002()
	{
		m_pipeline = Register<SI::APP002::Pipeline>(0);
		m_camera   = Register<SI::TrackballCamera>(0);
	}

	App002::~App002()
	{
	}
	
	void App002::OnPreUpdate   (const AppUpdateInfo& updateInfo)
	{
		m_pipeline->SetView( m_camera->GetView() );

		Super::OnPreUpdate(updateInfo);
	}

} // namespace APP002
} // namespace SI
