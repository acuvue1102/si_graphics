
#include "si_base/renderer/renderer.h"

namespace SI
{
	Renderer::Renderer()
		: Singleton<Renderer>(this)
	{
	}
	
	Renderer::~Renderer()
	{
	}
	
	void Renderer::Render(ModelInstancePtr& modelInstance)
	{
	}
	
} // namespace SI
