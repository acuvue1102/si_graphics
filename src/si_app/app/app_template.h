//
// app_template.h
///
// app‚Ì’èŒ^ˆ—‚ğˆ—‚·‚éƒNƒ‰ƒX. 
//
#pragma once

#include "si_base/core/core.h"
#include "si_app/app/app.h"
#include "si_app/app/app_observer.h"

namespace SI
{
	template<typename Observer>
	class SingleObserverApp
	{
	public:
		SingleObserverApp()
			: m_observer(SI_NEW(Observer, 0))
		{
		}

		~SingleObserverApp()
		{
			SI_DELETE(m_observer);
		}

		int Run(int nCmdShow)
		{			
			SI::AppDesc appDesc;
			appDesc.m_nCmdShow      = nCmdShow;
			appDesc.m_observers     = &m_observer;
			appDesc.m_observerCount = 1;

			SI::App app;
			return app.Run(appDesc);
		}

	public:
		SI::AppObserver* m_observer;
	};

} // namespace SI