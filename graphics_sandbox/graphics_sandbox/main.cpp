// main.cpp       Written by Keiji Matsuda in 2017

#include <Windows.h>
#include "app/app.h"

/* main function */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	SI::App app;
	
	SI::AppConfig appConfig;
	appConfig.m_nCmdShow = nCmdShow;
	app.Initialize(appConfig);

	int ret = app.Run();

	app.Terminate();

	return ret;
}
