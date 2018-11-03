
#include "si_base/file/file.h"
#include "si_base/file/file_win.h"

namespace SI
{
	File::File()
		: m_handle(nullptr)
	{
	}

	File::~File()
	{
		Close();
	}
		
	int File::Open(const char* filePath, FileAccessTypes access)
	{
		m_handle = FileBase::Open(filePath, access);
		
		if(!m_handle) return -1;

		return 0;
	}
	
	int64_t File::GetFileSize()
	{
		if(!m_handle) return -1;

		return FileBase::GetFileSize(m_handle);
	}
		
	int File::Read(
		void*    buffer,
		int64_t  size,
		int64_t* readSize)
	{
		if(!m_handle) return -1;
		
		return FileBase::Read(m_handle, buffer, size, readSize);
	}
	
	int File::Write(
		const void* buffer,
		int64_t  size,
		int64_t* writtenSize)
	{
		if(!m_handle) return -1;
		
		return FileBase::Write(m_handle, buffer, size, writtenSize);
	}

	int File::Seek(int64_t move, FilePosition position)
	{
		if(!m_handle) return -1;
		
		return FileBase::Seek(m_handle, move, position);
	}
		
	int File::Close()
	{
		if(!m_handle) return -1;

		void* handle = m_handle;
		m_handle = nullptr;
				
		return FileBase::Close(handle);
	}
		
	bool File::Exists(const char* filePath)
	{
		return FileBase::FileExists(filePath);
	}
	
	bool FileSystem::SetCurrentDir(const char* dir)
	{
		return FileBase::SetCurrentDir(dir);
	}
}
