
#include "app_004.h"

#include "pipeline_004.h"
#include "si_app/camera/trackball_camera.h"

namespace SI
{
namespace APP004
{
	//////////////////////////////////////////////////////////
	
	App004::App004()
	{
		m_pipeline = Register<SI::APP004::Pipeline>(0);
		m_camera   = Register<SI::TrackballCamera>(0);
	}

	App004::~App004()
	{
	}
	
	void App004::OnPreUpdate   (const AppUpdateInfo& updateInfo)
	{
		m_pipeline->SetView( m_camera->GetView() );

		Super::OnPreUpdate(updateInfo);
	}

} // namespace APP004
} // namespace SI
