#pragma once

#include <stdint.h>

namespace SI
{
	enum class MouseButton
	{
		Left = 0,
		Right,
		Middle,

		Max
	};

	struct MouseInfo
	{
		// 1つしか状態を保存してので、
		// 1フレーム中に押して離したりするとまずそうだが
		// 今は気にしない.
		int             m_x = 0;
		int             m_y = 0;
		int             m_wheel = 0; // 今のフレームで回転した移動量.
		bool            m_button[(uint32_t)MouseButton::Max] = {};
	};

	class Mouse
	{
	public:
		Mouse();
		~Mouse();

		void Flip();

	public:
		// マウスの位置取得
		int  GetX()                const{ return GetInfo().m_x;             }
		int  GetY()                const{ return GetInfo().m_y;             }
		int  GetWheel()            const{ return GetInfo().m_wheel;         }
		int  GetXPrev()            const{ return GetPreviousInfo().m_x;     }
		int  GetYPrev()            const{ return GetPreviousInfo().m_y;     }
		int  GetWheelPrev()        const{ return GetPreviousInfo().m_wheel; }

		// ボタンが押され始めたか.
		bool IsDown(MouseButton b) const
		{
			return GetInfo().m_button[(uint32_t)b] && !GetPreviousInfo().m_button[(uint32_t)b];
		}
		bool IsLDown()       const{ return IsDown(MouseButton::Left);   }
		bool IsRDown()       const{ return IsDown(MouseButton::Right);  }
		bool IsMDown()       const{ return IsDown(MouseButton::Middle); }

		// ボタンが解放されたか.
		bool IsUp(MouseButton b) const
		{
			return !GetInfo().m_button[(uint32_t)b] && GetPreviousInfo().m_button[(uint32_t)b];
		}
		bool IsLUp()         const{ return IsUp(MouseButton::Left);    }
		bool IsRUp()         const{ return IsUp(MouseButton::Right);   }
		bool IsMUp()         const{ return IsUp(MouseButton::Middle);  }
		
		// ボタンが連続で押されているか
		bool IsRepeat(MouseButton b) const
		{
			return GetInfo().m_button[(uint32_t)b] && GetPreviousInfo().m_button[(uint32_t)b];
		}
		bool IsLRepeat()     const{ return IsRepeat(MouseButton::Left);    }
		bool IsRRepeat()     const{ return IsRepeat(MouseButton::Right);   }
		bool IsMRepeat()     const{ return IsRepeat(MouseButton::Middle);  }

		// 現在のボタンの状態を取得.
		bool GetButton(MouseButton b) const
		{
			return GetInfo().m_button[(uint32_t)b];
		}
		bool GetLButton()     const{ return GetButton(MouseButton::Left);    }
		bool GetRButton()     const{ return GetButton(MouseButton::Right);   }		
		bool GetMButton()     const{ return GetButton(MouseButton::Middle);  }

		// 前のフレームのボタンの状態を取得.
		bool GetPreviousButton(MouseButton b) const
		{
			return GetPreviousInfo().m_button[(uint32_t)b];
		}
		bool GetPreviousLButton() const{ return GetPreviousButton(MouseButton::Left);    }
		bool GetPreviousRButton() const{ return GetPreviousButton(MouseButton::Right);   }		
		bool GetPreviousMButton() const{ return GetPreviousButton(MouseButton::Middle);  }

	public:
		void SetX(int x){ GetInfo().m_x = x; }
		void SetY(int y){ GetInfo().m_y = y; }
		void SetWheel(int wheel){ GetInfo().m_wheel = wheel; }
		void SetButton(MouseButton b, bool state)
		{
			GetInfo().m_button[(uint32_t)b] = state;
		}
		void SetLButton(bool state){ SetButton(MouseButton::Left,   state); }
		void SetRButton(bool state){ SetButton(MouseButton::Right,  state); }
		void SetMButton(bool state){ SetButton(MouseButton::Middle, state); }

	private:
		      MouseInfo& GetInfo()      { return m_info[m_infoIndex]; }
		const MouseInfo& GetInfo() const{ return m_info[m_infoIndex]; }
		
		      MouseInfo& GetPreviousInfo()      { return m_info[(m_infoIndex+1)%2]; }
		const MouseInfo& GetPreviousInfo() const{ return m_info[(m_infoIndex+1)%2]; }

	private:
		MouseInfo m_info[2];
		int       m_infoIndex;
	};

} // namespace SI
