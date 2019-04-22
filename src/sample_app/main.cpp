// main.cpp       Written by Keiji Matsuda in 2017

#include "si_base/platform/windows_proxy.h"
#include "si_base/core/core.h"
#include "si_app/app/app.h"
#include "app000_clear_color/pipeline_000.h"
#include "app001_simple_polygon/pipeline_001.h"
#include "app002_texture/app_002.h"
#include "app003_compute/pipeline_003.h"
#include "app004_renderer/app_004.h"
#include "app005_raytracing_in_one_weekend/pipeline_005.h"
#include "app006_raytracing/pipeline_006.h"

/* main function */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	int mode = 6;
	
	SI::AppDesc appDesc;
	appDesc.m_nCmdShow = nCmdShow;

	switch(mode)
	{
	case 0:
	{
		SI::App app;
		app.Register<SI::APP000::Pipeline>(0);
		return app.Run(appDesc);
	}
	case 1:
	{
		SI::App app;
		app.Register<SI::APP001::Pipeline>(0);
		return app.Run(appDesc);
	}
	case 2:
	{
		SI::APP002::App002 app;
		return app.Run(appDesc);
	}
	case 3:
	{
		SI::App app;
		app.Register<SI::APP003::Pipeline>(0);
		return app.Run(appDesc);
	}
	case 4:
	{
		SI::APP004::App004 app;
		return app.Run(appDesc);
	}
	case 5:
	{
		SI::App app;
		app.Register<SI::APP005::Pipeline>(0);
		return app.Run(appDesc);
	}
	case 6:
	{
		SI::App app;
		app.Register<SI::APP006::Pipeline>(0);
		return app.Run(appDesc);
	}
	default:
		break;
	}

	return 0;
}
