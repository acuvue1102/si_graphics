#pragma once

namespace SI
{
	class Model;
	class ModelReader
	{
	public:
		ModelReader();
		~ModelReader();

		bool Read(Model& model, const char* path);
	};

} // namespace SI
