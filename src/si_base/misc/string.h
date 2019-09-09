#pragma once

#include <string>
#include <algorithm>

namespace SI
{
	class String : public std::string
	{
	public:
		String() : std::string() {}
		String(const char *s) : std::string(s) {}
		String(const std::string &s) : std::string(s) {}
		String(std::string &&s) noexcept : std::string(std::move(s)) {}

		String& operator=(const char *s) 
		{
			std::string::operator=(s);
			return *this;
		}

		String& operator=(const String& s) 
		{
			std::string::operator=(s);
			return *this;
		}

		String ToLower() const
		{
			String ret = *this;
			std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);

			return ret;
		}
	};

} // namespace SI
