#pragma once

#include <cstdint>
#include <si_app/pipeline/pipeline_base.h>

namespace SI
{
namespace APP000
{
	class Pipeline  : public PipelineBase
	{
	public:
		explicit Pipeline(int observerSortKey);
		virtual ~Pipeline();

		int  OnInitialize(const AppInitializeInfo&) override;
		int  OnTerminate()                          override;
		void OnRender(const App& app, const AppUpdateInfo&)         override;
	};

} // namespace APP000
} // namespace SI