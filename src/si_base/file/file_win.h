#pragma once

#include "si_base/platform/windows_proxy.h"
#include "Shlwapi.h"

#include "si_base/core/assert.h"
#include "si_base/core/basic_function.h"
#include "si_base/file/file.h"

#pragma comment(lib, "Shlwapi.lib")

namespace SI
{
	struct FileBase
	{
		inline static void* Open(const char* filePath, FileAccessTypes access)
		{
			DWORD dwAccess    = 0;
			DWORD dwShareMode = FILE_SHARE_READ;
			DWORD dwCreationDisposition = 0;

			if(access & FileAccessType::Read)
			{
				dwAccess |= GENERIC_READ;
				dwCreationDisposition |= OPEN_EXISTING;
			}

			if(access & FileAccessType::Write)
			{
				dwAccess |= GENERIC_WRITE;
			}

			char canonnicalizedFilePath[MAX_PATH];
			PathCanonicalizeA(canonnicalizedFilePath, filePath);
			
			HANDLE h = CreateFileA(
				canonnicalizedFilePath,
				dwAccess,
				dwShareMode,
				NULL,
				dwCreationDisposition,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			if(h==INVALID_HANDLE_VALUE) return nullptr;

			return h;
		}

		inline static int64_t GetFileSize(void* handle)
		{
			HANDLE h = (HANDLE)handle;
			LARGE_INTEGER fileSize;

			BOOL ret = GetFileSizeEx(h, &fileSize);
			
			return (ret!=FALSE)? fileSize.QuadPart : -1;
		}

		
		inline static int Read(
			void*    handle,
			void*    buffer,
			int64_t  size,
			int64_t* readSize = nullptr)
		{
			SI_ASSERT(buffer);
			SI_ASSERT(0<size && size < 0xffffffff);

			DWORD dwReadSize = 0;

			HANDLE h = (HANDLE)handle;
			BOOL ret = ReadFile(
				h,
				buffer,
				(DWORD)size,
				&dwReadSize,
				NULL);

			if((ret==FALSE))
			{
				return -1;
			}

			if(readSize)
			{
				*readSize = dwReadSize;
			}

			return 0;
		}

		inline static int Seek(
			void*   handle,
			int64_t move,
			FilePosition position = FilePosition::Current)
		{
			static const DWORD kMethodTable[] =
			{
				FILE_BEGIN,
				FILE_CURRENT,
				FILE_END,
			};
			SI_ASSERT((uint32_t)position < (uint32_t)ArraySize(kMethodTable));

			LARGE_INTEGER distance;
			distance.QuadPart = move;

			HANDLE h = (HANDLE)handle;
			BOOL ret = SetFilePointerEx(
				h,
				distance,
				NULL,
				kMethodTable[(uint32_t)position]);

			return (ret!=FALSE)? 0 : -1;
		}
		
		inline static int Close(void*   handle)
		{
			HANDLE h = (HANDLE)handle;
			BOOL ret = CloseHandle(h);

			return (ret!=FALSE)? 0 : -1;
		}
	};
}
