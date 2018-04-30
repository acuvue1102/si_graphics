
#include <stdio.h>
#include <algorithm>
#include "si_base/core/core.h"
#include "si_base/platform/window_util.h"
#include "si_app/file/path_storage.h"
#include "si_app/app/app_observer.h"

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
		Terminate();
	}

	int App::Run(const AppDesc& desc)
	{
		if(Initialize(desc) != 0)
		{
			return -1;
		}

		while(true)
		{
			Update();

			Render();

			if(WindowUtil::MessageProc() != 0)
			{
				break;
			}
		}

		if(Terminate() != 0)
		{
			return -1;
		}

		return 0;
	}

	int App::Initialize(const AppDesc& desc)
	{
		if(m_initialized) return 0;

		if(desc.m_observerCount<=0 || desc.m_observers == nullptr)
		{
			SI_ASSERT(0);
			return -1;
		}

		m_pathStorage = SI_NEW(PathStorage);
		m_pathStorage->Initialize();

		void* hWnd = WindowUtil::Initialize((int)desc.m_width,	(int)desc.m_height);
		if(hWnd==nullptr) return -1;


		// obserberの初期化.
		{
			m_objservers.reserve(desc.m_observerCount);
			for(uint32_t o=0; o<desc.m_observerCount; ++o)
			{
				m_objservers.push_back(desc.m_observers[o]);
			}

			std::sort(m_objservers.begin(), m_objservers.end(),
				[](const SI::AppObserver* left, const SI::AppObserver* right)->bool
				{
					return (left->GetSortKey() < right->GetSortKey()); // 小さい順にソート.
				});

			AppInitializeInfo initializeInfo;
			initializeInfo.m_width  = desc.m_width;
			initializeInfo.m_height = desc.m_height;
			initializeInfo.m_hWnd   = hWnd;
			for(AppObserver* obs : m_objservers)
			{
				obs->OnInitialize(initializeInfo);
			}
		}

		if(WindowUtil::ShowWindow(hWnd, desc.m_nCmdShow) != 0) return -1;

		m_initialized = true;
		return 0;
	}

	int App::Terminate()
	{
		if(!m_initialized) return 0;
		
		for(AppObserver* obs : m_objservers)
		{
			obs->OnTerminate();
		}
		m_objservers.clear();

		if(WindowUtil::Terminate() != 0)
		{
			return -1;
		}

		if(m_pathStorage)
		{
			m_pathStorage->Terminate();
			SI_DELETE(m_pathStorage);
		}

		m_initialized = false;
		return 0;
	}
	
	void App::Update()
	{
		AppUpdateInfo updateInfo;
		for(AppObserver* obs : m_objservers)
		{
			obs->OnUpdate(updateInfo);
		}
	}

	void App::Render()
	{
		AppRenderInfo renderInfo;
		for(AppObserver* obs : m_objservers)
		{
			obs->OnRender(renderInfo);
		}
	}
} // namespace SI
