//
// app.h
///
// appクラス. 
// observerを登録して、Runを呼ぶ
//
#pragma once

#include <cstdint>
#include <vector>
#include <si_base/core/new_delete.h>
#include "si_base/platform/window_app.h"

namespace SI
{
	class  PathStorage;
	class  AppModule;
	struct AppUpdateInfo;

	struct AppDesc
	{
		uint32_t       m_width         = 1280;
		uint32_t       m_height        = 720;
		int            m_nCmdShow      = 0;
	};

	class App : public AppPlatformBase
	{
	public:
		App();
		virtual ~App();

		template<typename AppModuleT, class... Arg>
		AppModuleT* Register(Arg... arg)
		{
			static_assert(AppModuleT::classValue == AppModule::classValue, "基底クラスがAppModuleじゃない.");

			// デストラクタで消されるので開放不要.
			AppModuleT* m = SI_NEW(AppModuleT, arg...);
			m_modules.push_back(m);
			return m;
		}

		int Run(const AppDesc& desc);

	protected:
		virtual int  OnInitialize(const AppDesc& desc);
		virtual int  OnTerminate();

		virtual void OnPreUpdate   (const AppUpdateInfo& updateInfo);
		virtual void OnUpdate      (const AppUpdateInfo& updateInfo);
		virtual void OnRender      (const AppUpdateInfo& updateInfo);

		virtual void OnMouseMove   (int x, int y)               override;
		virtual void OnMouseButton (MouseButton b, bool isDown) override;
		virtual void OnMouseWheel  (int wheel)                  override;
		virtual void OnKeyboard    (Key k, bool isDown)         override;

	private:
		PathStorage*                m_pathStorage;
		std::vector<AppModule*>     m_modules;
		bool                        m_initialized;
	};

} // namespace SI