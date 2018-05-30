
#include "si_base/input/mouse.h"

#include <stdarg.h>
#include <Windows.h>
#include <stdio.h>

namespace SI
{
	Mouse::Mouse()
		: m_infoIndex(0)
	{
	}

	Mouse::~Mouse()
	{
	}

	void Mouse::Flip()
	{
		m_infoIndex = (m_infoIndex + 1)%2;
		m_info[m_infoIndex] = GetPreviousInfo();

		m_info[m_infoIndex].m_wheel = 0; // wheelだけは状態を受け継がない.
	}
}
