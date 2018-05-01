// main.cpp       Written by Keiji Matsuda in 2017

#include <Windows.h>
#include "si_base/core/core.h"
#include "si_app/app/app_template.h"
#include "app000_clear_color/pipeline_000.h"
#include "app001_simple_polygon/pipeline_001.h"

/* main function */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	int mode = 1;

	switch(mode)
	{
	case 0:
	{
		SI::SingleObserverApp<SI::APP000::Pipeline> app;
		return app.Run(nCmdShow);
	}
	case 1:
	{
		SI::SingleObserverApp<SI::APP001::Pipeline> app;
		return app.Run(nCmdShow);
	}
	default:
		break;
	}

	return 0;
}
