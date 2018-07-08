
#include "si_base/input/keyboard.h"

#include <stdarg.h>
#include <stdio.h>
#include "si_base/platform/windows_proxy.h"

namespace SI
{
	Keyboard::Keyboard()
	{
	}
	
	Keyboard::~Keyboard()
	{
	}
	
	void Keyboard::Flip()
	{
		m_infoIndex = (m_infoIndex + 1)%2;
		m_info[m_infoIndex] = GetPreviousInfo();
	}
	
	void Keyboard::UpdateModifier()
	{
		ModifierKeys keys = ModifierKey::None;
		if(GetInfo().m_key[(uint32_t)Key::Ctrl])  keys |= ModifierKey::Ctrl;
		if(GetInfo().m_key[(uint32_t)Key::Alt])   keys |= ModifierKey::Alt;
		if(GetInfo().m_key[(uint32_t)Key::Shift]) keys |= ModifierKey::Shift;
	}
}
