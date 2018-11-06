
#include "si_base/renderer/model_reader.h"

#include <picojson/picojson.h>

#include "si_base/renderer/model.h"
#include "si_base/renderer/mesh.h"
#include "si_base/renderer/sub_mesh.h"
#include "si_base/renderer/material.h"
#include "si_base/renderer/geometry.h"
#include "si_base/math/math.h"
#include "si_base/core/print.h"
#include "si_base/file/file.h"
#include "si_base/serialization/deserializer.h"

namespace SI
{
	ModelReader::ModelReader()
	{
	}
	
	ModelReader::~ModelReader()
	{
	}

	bool ModelReader::Read(ModelInstancePtr& modelInstance, const char* path)
	{
		DeserializedObject deserializedObject;
		Deserializer deserializer;
		deserializer.Initialize();
		bool ret = deserializer.Deserialize<ModelSerializeData>(deserializedObject, path);
		deserializer.Terminate();

		if(!ret) return false;

		const ModelSerializeData& serializeData = *deserializedObject.Get<ModelSerializeData>();

		modelInstance = std::make_shared<ModelInstance>();
		modelInstance->GetModel()->ImportSerializeData(serializeData);
			
		return true;
	}

} // namespace SI
