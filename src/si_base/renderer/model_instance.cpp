
#include "si_base/renderer/model_instance.h"

namespace SI
{
	ModelInstance::ModelInstance()
	{
	}

	ModelInstance::~ModelInstance()
	{
	}

	void ModelInstance::SetModel(ModelPtr& model)
	{
		m_model = model;
	}

} // namespace SI
