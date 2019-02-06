
#include <stdio.h>
#include <algorithm>
#include "si_base/platform/windows_proxy.h"
#include "si_base/core/core.h"
#include "si_base/file/file.h"
#include "si_app/file/path_storage.h"
#include "si_app/app/app_module.h"

#include "app.h"

namespace SI
{
	App::App()
		: m_pathStorage(nullptr)
		, m_initialized(false)
	{
	}

	App::~App()
	{
		SI_ASSERT(m_modules.size()==0);
		SI_ASSERT(m_pathStorage==nullptr);
	}

	int App::Run(const AppDesc& desc)
	{
		if(OnInitialize(desc) != 0)
		{
			return -1;
		}

		while(UpdateBase() == 0)
		{
			AppUpdateInfo updateInfo;

			OnPreUpdate(updateInfo);

			OnUpdate(updateInfo);

			OnRender(updateInfo);
		}

		if(OnTerminate() != 0)
		{
			return -1;
		}

		return 0;
	}
	
	int App::OnInitialize(const AppDesc& desc)
	{
		if(m_initialized) return 0;

		SI::FileSystem::SetCurrentDir(SI_APP_PROJECT_DIR "..\\..\\");

		m_pathStorage = SI_NEW(PathStorage);
		m_pathStorage->Initialize();

		InitializeBase((int)desc.m_width,	(int)desc.m_height);


		// obserberの初期化.
		{
			std::sort(m_modules.begin(), m_modules.end(),
				[](const SI::AppModule* left, const SI::AppModule* right)->bool
				{
					return (left->GetSortKey() < right->GetSortKey()); // 小さい順にソート.
				});

			AppInitializeInfo initializeInfo;
			initializeInfo.m_width  = desc.m_width;
			initializeInfo.m_height = desc.m_height;
			initializeInfo.m_hWnd   = GetWindowHandle();
			for(AppModule* m : m_modules)
			{
				int ret = m->OnInitialize(initializeInfo);
				SI_ASSERT(ret==0);
			}
		}

		if(ShowWindow(desc.m_nCmdShow) != 0) return -1;

		m_initialized = true;
		return 0;
	}

	int App::OnTerminate()
	{
		if(!m_initialized) return 0;
		
		for(AppModule* m : m_modules)
		{
			m->OnTerminate();
			SI_DELETE(m);
		}
		m_modules.clear();

		TerminateBase();

		if(m_pathStorage)
		{
			m_pathStorage->Terminate();
			SI_DELETE(m_pathStorage);
		}

		m_initialized = false;
		return 0;
	}
	
	void App::OnPreUpdate(const AppUpdateInfo& updateInfo)
	{
		for(AppModule* m : m_modules)
		{
			m->OnPreUpdate(*this, updateInfo);
		}
	}
	
	void App::OnUpdate(const AppUpdateInfo& updateInfo)
	{
		for(AppModule* m : m_modules)
		{
			m->OnUpdate(*this, updateInfo);
		}
	}

	void App::OnRender(const AppUpdateInfo& updateInfo)
	{
		for(AppModule* m : m_modules)
		{
			m->OnRender(*this, updateInfo);
		}
	}
	
	void App::OnMouseMove(int x, int y)
	{
		for(AppModule* m : m_modules)
		{
			m->OnMouseMove(*this, x, y);
		}
	}

	void App::OnMouseButton(MouseButton b, bool isDown)
	{
		for(AppModule* m : m_modules)
		{
			m->OnMouseButton(*this, b, isDown);
		}
	}
	
	void App::OnMouseWheel(int wheel)
	{
		for(AppModule* m : m_modules)
		{
			m->OnMouseWheel(*this, wheel);
		}
	}

	void App::OnKeyboard(Key k, bool isDown)
	{
		for(AppModule* m : m_modules)
		{
			m->OnKeyboard(*this, k, isDown);
		}
	}
} // namespace SI
