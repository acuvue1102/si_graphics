#pragma once

#include <stdint.h>
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

		int OnInitialize(const AppInitializeInfo&) override;
		int OnTerminate()                          override;
		int OnRender(const AppRenderInfo&)         override;
	};

} // namespace APP000
} // namespace SI