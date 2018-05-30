#pragma once

#include "si_base/misc/enum_flags.h"

namespace SI
{
	// For Japanese Keyboard
	enum class Key
	{
		None         = 0,

		Comma        = ',',       // 44 ,<
		Hyphen       = '-',       // 45 -=
		Dot          = '.',       // 46 .>
		Slash        = '/',       // 47 /?

		Num0 = '0',               // 48
		Num1 = '1',
		Num2 = '2',
		Num3 = '3',
		Num4 = '4',
		Num5 = '5',
		Num6 = '6',
		Num7 = '7',
		Num8 = '8',
		Num9 = '9',
		
		Colon         = ':',       // 58 :*
		Semicolon     = ';',       // 59 ;+
		At            = '@',       // 64 @`

		A = 'A',                   // 65
		B = 'B',
		C = 'C',
		D = 'D',
		E = 'E',
		F = 'F',
		G = 'G',
		H = 'H',
		I = 'I',
		J = 'J',
		K = 'K',
		L = 'L',
		M = 'M',
		N = 'N',
		O = 'O',
		P = 'P',
		Q = 'Q',
		R = 'R',
		S = 'S',
		T = 'T',
		U = 'U',
		V = 'V',
		W = 'W',
		X = 'X',
		Y = 'Y',
		Z = 'Z',                  // 90
		
		LeftBracket    =  '[',    // 91 [{
		BackSlash      = '\\',    // 92 \_
		RightBracket   =  ']',    // 93 ]}
		Carret         = '^',     // 94 ^~
		
		// system key...
		SystemKeyStart = 128,
		F1 = SystemKeyStart,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		Shift,
		Alt,
		Ctrl,
		Space,
		Enter,
		Tab,
		Caps,
		Del,
		Backspace,
		Up,
		Down,
		Left,
		Right,
		Esc,
		Insert,

		Max,
	};

	enum class ModifierKey
	{
		Uncontrol = -1,  // Modifierに関わらず反応させたい場合に使う特殊なタイプ.
		None      = 0,
		Ctrl      = 1<<0,
		Shift     = 1<<1,
		Alt       = 1<<2,
	};
	SI_DECLARE_ENUM_FLAGS(ModifierKeys, ModifierKey);
		
	struct KeyboardInfo
	{
		// 1つしか状態を保存してので、
		// 1フレーム中に押して離したりするとまずそうだが
		// 今は気にしない.
		bool             m_key[(uint32_t)Key::Max];
		ModifierKey      m_modifier;

		KeyboardInfo()
			: m_modifier(ModifierKey::None)
		{
			for(bool& key : m_key){ key = false; }
		}
	};

	class Keyboard
	{
	public:
		Keyboard();
		~Keyboard();

		void Flip();

		void UpdateModifier();

	public:
		// 押され始めたか
		bool IsKeyDown(Key key, ModifierKeys modifier = ModifierKey::None) const
		{
			if(modifier != ModifierKey::Uncontrol)
			{
				if( modifier == GetPreviousInfo().m_modifier &&
					GetPreviousInfo().m_key[(uint32_t)key] )
				{
					return false; // 前のフレームで押されてた.
				}
				
				if(modifier != GetInfo().m_modifier) return false;
			}
			else
			{
				if( GetPreviousInfo().m_key[(uint32_t)key] ) return false; // 前のフレームで押されてた.
			}

			return GetInfo().m_key[(uint32_t)key];
		}
		
		// 押されていたキーが離されたか
		bool IsKeyUp(Key key, ModifierKeys modifier = ModifierKey::None) const
		{
			if(!GetPreviousInfo().m_key[(uint32_t)key])  return false;
			if(modifier != ModifierKey::Uncontrol)
			{
				if(modifier != GetPreviousInfo().m_modifier) return false;

				// Modifierが離された時.
				if(modifier != GetInfo().m_modifier) return true;
			}

			return !GetInfo().m_key[(uint32_t)key];
		}

		// 現在のキーボードの状態を取得する.
		bool GetKeyState(Key key, ModifierKeys modifier = ModifierKey::None) const
		{
			if(modifier != ModifierKey::Uncontrol)
			{
				if(modifier != GetInfo().m_modifier) return false;
			}

			return GetInfo().m_key[(uint32_t)key];
		}

	public:
		void SetKey(Key key, bool state)
		{
			GetInfo().m_key[(uint32_t)key] = state;
		}

	private:
		      KeyboardInfo& GetInfo()      { return m_info[m_infoIndex]; }
		const KeyboardInfo& GetInfo() const{ return m_info[m_infoIndex]; }
		
		      KeyboardInfo& GetPreviousInfo()      { return m_info[(m_infoIndex+1)%2]; }
		const KeyboardInfo& GetPreviousInfo() const{ return m_info[(m_infoIndex+1)%2]; }

	private:
		KeyboardInfo m_info[2];
		int          m_infoIndex;
	};

} // namespace SI
