#pragma once

namespace SI
{
	class Model;
	class ModelReaderImpl;

	class ModelReader
	{
	public:
		ModelReader();
		~ModelReader();

		bool Read(Model& model, const char* path);

	private:
		ModelReaderImpl* m_impl;
	};

} // namespace SI
