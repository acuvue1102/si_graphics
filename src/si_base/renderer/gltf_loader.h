#pragma once

#include <memory>
#include <vector>
#include "si_base/core/assert.h"
#include "si_base/misc/string.h"
#include "si_base/renderer/scenes.h"

namespace SI
{
	class GltfLoaderImpl;
	class GltfLoader
	{
	public:
		GltfLoader();
		~GltfLoader();

		ScenesPtr Load(const char* filePath);

	private:
		GltfLoaderImpl* m_impl;
	};

} // namespace SI
