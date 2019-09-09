#pragma once

namespace SI
{
	struct ModelSerializeData;

	class ModelWriter
	{
	public:
		ModelWriter();
		~ModelWriter();

		bool Write(const char* path, const ModelSerializeData& serializeData);
	};

} // namespace SI
