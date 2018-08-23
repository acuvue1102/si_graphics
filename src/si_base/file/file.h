#pragma once

#include "si_base/misc/enum_flags.h"

namespace SI
{
	enum class FileAccessType
	{
		Read   = 1<<0,
		Write  = 1<<1,
	};
	SI_DECLARE_ENUM_FLAGS(FileAccessTypes, FileAccessType);


	enum class FilePosition
	{
		Begin,
		Current,
		End
	};

	class File
	{
	public:
		File();
		~File();
		
		int Open(const char* filePath, FileAccessTypes access = FileAccessType::Read);
		
		int64_t GetFileSize();

		int Read(
			void*    buffer,
			int64_t  size,
			int64_t* readSize = nullptr);

		int Write(
			const void* buffer,
			int64_t  size,
			int64_t* readSize = nullptr);

		int Seek(int64_t move, FilePosition position = FilePosition::Current);
		
		int Close();

		
	public:
		static bool Exists(const char* filePath);

	private:
		void* m_handle;
	};
}
