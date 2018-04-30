// main.cpp       Written by Keiji Matsuda in 2017

#include <Windows.h>
#include "si_base/core/core.h"
#include "si_app/app/app_template.h"
#include "pipeline.h"

/* main function */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	SI::SingleObserverApp<SI::Pipeline> app;
	return app.Run(nCmdShow);
}
