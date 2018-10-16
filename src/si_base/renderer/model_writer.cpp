
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
	class ModelWriterImpl
	{
	public:		
		bool Write(const char* path, const Model& model)
		{
			ModelSerializeData serializeData = model.ConvertSerializeData();

			Serializer serializer;
			serializer.Initialize();
			serializer.Serialize(path, serializeData);
			serializer.Terminate();

			return true;
		}
	};

	/////////////////////////////////////////////////////////////////////////////

	ModelWriter::ModelWriter()
		: m_impl(SI_NEW(ModelWriterImpl))
	{
	}
	
	ModelWriter::~ModelWriter()
	{
		SI_DELETE(m_impl);
	}

	bool ModelWriter::Write(const char* path, const Model& model)
	{
		return m_impl->Write(path, model);
	}

} // namespace SI
