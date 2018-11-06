
#include "si_base/renderer/model_instance.h"

namespace SI
{
	ModelInstance::ModelInstance()
		: m_model( std::make_shared<Model>() )
	{
	}

	ModelInstance::ModelInstance(ModelPtr& model)
		: m_model(model)
	{
	}

	ModelInstance::~ModelInstance()
	{
	}

	//void ModelInstance::SetModel(ModelPtr& model)
	//{
	//	m_model = model;
	//}

} // namespace SI
