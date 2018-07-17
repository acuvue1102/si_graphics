/////////////////////////////////////////
// path_storage.h
//
// よく使うパスをまとめたクラス.
// シングルトンでどこからでもアクセスできるようにしておく.
/////////////////////////////////////////
#pragma once

#include <vector>
#include <cstdint>
#include "si_base/file/file.h"

namespace SI
{
	struct FileUtility
	{
		inline static int Load(std::vector<uint8_t>& outBuffer, const char* filePath)
		{
			File file;
			int ret = file.Open(filePath);
			if(ret != 0) return -1;

			int64_t fileSize = file.GetFileSize();
			if(fileSize <=0) return -1;

			outBuffer.resize((size_t)fileSize);

			int64_t readSize = 0;
			ret = file.Read(&outBuffer[0], fileSize, &readSize);
			if(ret != 0) return -1;
			if(fileSize != readSize) return -1;

			file.Close();

			return 0;
		}

	};

} // namespace SI
