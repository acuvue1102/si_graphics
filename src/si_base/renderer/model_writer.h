#pragma once

namespace SI
{
	class Model;
	class ModelWriterImpl;

	class ModelWriter
	{
	public:
		ModelWriter();
		~ModelWriter();

		bool Write(const char* path, const Model& model);

	private:
		ModelWriterImpl* m_impl;
	};

} // namespace SI
