
#include "si_base/misc/argument_parser.h"

#include <stdlib.h>
#include "si_base/misc/string_util.h"

namespace SI
{
	ArgumentParser::ArgumentParser()
		: m_argc(0)
		, m_argv(nullptr)
	{
	}

	ArgumentParser::~ArgumentParser()
	{
	}

	void ArgumentParser::Initialize(int argc, const char* const* argv)
	{
		m_argc = argc;
		m_argv = argv;
	}

	bool ArgumentParser::Exists(const char* arg) const
	{
		if(arg==nullptr) return false;

		for(int i=1; i<m_argc; ++i)
		{
			if( StringUtil::IsSameNoCase(m_argv[i], arg) )
			{
				return true;
			}
		}

		return false;
	}

	const char* ArgumentParser::GetAsString(const char* arg, const char* defaultStr) const
	{
		if(arg==nullptr) return defaultStr;

		int argMinus1 = m_argc-1;

		for(int i=1; i<argMinus1; ++i)
		{
			if( StringUtil::IsSameNoCase(m_argv[i], arg) )
			{
				return m_argv[i+1];
			}
		}

		return defaultStr;
	}

	int ArgumentParser::GetAsInt(const char* arg, int defaultValue) const
	{
		if(arg==nullptr) return defaultValue;

		int argMinus1 = m_argc-1;

		for(int i=1; i<argMinus1; ++i)
		{
			if( StringUtil::IsSameNoCase(m_argv[i], arg) )
			{
				return atoi(m_argv[i+1]);
			}
		}

		return defaultValue;
	}

	float ArgumentParser::GetAsFloat(const char* arg, float defaultValue) const
	{
		if(arg==nullptr) return defaultValue;

		int argMinus1 = m_argc-1;

		for(int i=1; i<argMinus1; ++i)
		{
			if( StringUtil::IsSameNoCase(m_argv[i], arg) )
			{
				return (float)atof(m_argv[i+1]);
			}
		}

		return defaultValue;
	}

} // namespace SI
