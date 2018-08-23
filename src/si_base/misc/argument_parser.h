#pragma once

#include <cstdint>

namespace SI
{
	class ArgumentParser
	{
	public:
		ArgumentParser();
		~ArgumentParser();

		void Initialize(int argc, const char* const* argv);
		
		bool        Exists     (const char* arg) const;
		const char* GetAsString(const char* arg, const char* defaultStr = nullptr) const;
		int         GetAsInt   (const char* arg, int defaultValue = 0) const;
		float       GetAsFloat (const char* arg, float defaultValue = 0.0f) const;

	private:
		int m_argc;
		const char* const* m_argv;
	};

} // namespace SI
