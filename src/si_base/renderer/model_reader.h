#pragma once

#include "si_base/renderer/model_instance.h"

namespace SI
{
	class Model;
	class ModelReader
	{
	public:
		ModelReader();
		~ModelReader();

		bool Read(ModelInstancePtr& model, const char* path);
	};

} // namespace SI
