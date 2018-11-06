#pragma once

#include "si_base/renderer/renderer_common.h"
#include "si_base/container/array.h"
#include "si_base/core/singleton.h"
#include "si_base/renderer/model_instance.h"

namespace SI
{
	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer();
		~Renderer();

		void Render(ModelInstancePtr& modelInstance);

	private:
	};

} // namespace SI
