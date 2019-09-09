
#include "si_base/file/path.h"

namespace SI
{
	String PathUtility::GetExt(const String& path)
	{
		size_t pos = path.find_last_of('.');

		if(std::string::npos == pos) return String();

		return path.substr(pos);
	}
}
