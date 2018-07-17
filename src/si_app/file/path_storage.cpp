
#include "si_app/file/path_storage.h"

#include "si_base/platform/windows_proxy.h"
#include <si_base/core/core.h>

namespace SI
{
	PathStorage::PathStorage()
		: Singleton(this)
	{
		m_exePath[0] = 0;
		m_exeDirPath[0] = 0;
	}
	
	PathStorage::~PathStorage()
	{
	}

	void PathStorage::Initialize()
	{
		DWORD size = GetModuleFileNameA(nullptr, m_exePath, (DWORD)ArraySize(m_exePath));
		if (size == 0 || (DWORD)ArraySize(m_exePath) <= size)
		{
			SI_ASSERT(0);
			m_exePath[0] = 0;
			m_exeDirPath[0] = 0;
			return;
		}
		SI_ASSERT(m_exePath[size] == '\0');

		char* lastSlash = nullptr;
		for(DWORD c=0; c<size; ++c)
		{
			m_exeDirPath[c] = m_exePath[c];
			if(m_exeDirPath[c] == '\\' || m_exeDirPath[c] == '/')
			{
				lastSlash = &m_exeDirPath[c];
			}
		}
		if(lastSlash)
		{
			++lastSlash; // 最後のセパレータを含める.
			if(lastSlash < &m_exeDirPath[kPathMax]) // 念のため.
			{
				*lastSlash = 0;
			}
		}

		sprintf_s(m_assetDirPath, "%s..\\..\\..\\asset\\", m_exeDirPath);
	}

	void PathStorage::Terminate()
	{
		m_exePath[0] = 0;
		m_exeDirPath[0] = 0;
	}

} // namespace SI
