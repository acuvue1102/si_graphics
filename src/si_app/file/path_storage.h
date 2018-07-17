/////////////////////////////////////////
// path_storage.h
//
// よく使うパスをまとめたクラス.
// シングルトンでどこからでもアクセスできるようにしておく.
/////////////////////////////////////////
#pragma once

#include "si_base/core/singleton.h"

namespace SI
{
	class PathStorage : public Singleton<PathStorage>
	{
	public:
		PathStorage();
		~PathStorage();

		void Initialize();
		void Terminate();
		
		const char* GetExePath()      const{ return m_exePath; }
		const char* GetExeDirPath()   const{ return m_exeDirPath; }
		const char* GetAssetDirPath() const{ return m_assetDirPath; }


		static const size_t kPathMax = 260;

	private:
		char m_exePath   [kPathMax];    // exeのパス.
		char m_exeDirPath[kPathMax];    // exeが置かれているパス.
		char m_assetDirPath[kPathMax];  // assetのパス.
	};
} // namespace SI

#define SI_PATH_STORAGE() (*PathStorage::GetInstance())
