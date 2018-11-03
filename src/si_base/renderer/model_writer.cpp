
#include "si_base/renderer/model_writer.h"

#include <picojson/picojson.h>

#include "si_base/renderer/model.h"
#include "si_base/renderer/mesh.h"
#include "si_base/renderer/sub_mesh.h"
#include "si_base/renderer/material.h"
#include "si_base/renderer/geometry.h"
#include "si_base/math/math.h"
#include "si_base/core/print.h"
#include "si_base/file/file.h"
#include "si_base/serialization/serializer.h"

namespace SI
{
	ModelWriter::ModelWriter()
	{
	}
	
	ModelWriter::~ModelWriter()
	{
	}

	bool ModelWriter::Write(const char* path, const ModelSerializeData& serializeData)
	{
		Serializer serializer;
		serializer.Initialize();
		bool ret = serializer.Serialize(path, serializeData);
		serializer.Terminate();

		return ret;
	}

} // namespace SI
