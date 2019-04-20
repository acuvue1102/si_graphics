
#include "si_app/camera/trackball_camera.h"

#include "si_app/app/app.h"

namespace SI
{
	TrackballCamera::TrackballCamera(int sortKey)
		: AppModule(sortKey)
		, m_focus(0.0, 0.0, 10.0f)
	{
	}
	
	TrackballCamera::~TrackballCamera()
	{
	}
	
	Vfloat4x4 TrackballCamera::GetView() const
	{
		return m_view;
	}
	
	void TrackballCamera::OnPreUpdate(const App& app, const AppUpdateInfo&)
	{
		Vfloat3 cameraTarget = m_targetPos;
		
		Vfloat3 cameraPos = (-m_focus) * m_quat + m_targetPos;
		Vfloat3 cameraUp  = Vfloat3::AxisY() * m_quat;

		m_view = Math::LookAtMatrix(cameraPos, cameraTarget, cameraUp);
	}

	void TrackballCamera::OnMouseMove(const App& app, int x, int y)
	{
		bool isAlt = app.GetKeyboard().GetKeyState(Key::Alt, SI::ModifierKey::Uncontrol);
		if(!isAlt) return;

		const Mouse& mouse = app.GetMouse();

		int prevX = mouse.GetXPrev();
		int prevY = mouse.GetYPrev();

		int diffX = x - prevX;
		int diffY = y - prevY;
		
		bool isLeftClick = mouse.GetLButton();
		if(isLeftClick)
		{
			OnRotate(diffX, diffY);
			return;
		}

		bool isMiddleClick = mouse.GetMButton();
		if(isMiddleClick)
		{
			OnTranslate(diffX, diffY);
			return;
		}

		bool isRightClick = mouse.GetRButton();
		if(isRightClick)
		{
			OnZoom(diffY);
			return;
		}
	}

	void TrackballCamera::OnMouseButton (const App& app, MouseButton b, bool isDown)
	{
	}

	void TrackballCamera::OnMouseWheel(const App& app, int wheel)
	{
		OnZoom(wheel);
	}
	
	void TrackballCamera::OnKeyboard(const App& app, Key k, bool isDown)
	{
		if(k == Key::F && isDown)
		{
			m_targetPos = Vfloat3::Zero();
		}
	}
	
	void TrackballCamera::OnRotate(int diffX, int diffY)
	{
		float diffXf = -0.01f * (float)diffX;
		float diffYf = -0.01f * (float)diffY;
				
		Vfloat3 axisX = Vfloat3::AxisX() * m_quat;
		Vfloat3 axisY = Vfloat3::AxisY() * m_quat;

		Vfloat3 newAxis = diffYf * axisX + diffXf * axisY;
		Vfloat length = newAxis.Length();
		if(length <= 0.00001f) return;

		Vquat rot(newAxis/length, -length);
		rot = rot.Normalize();

		m_quat *= rot;
		m_quat = m_quat.Normalize();
	}

	void TrackballCamera::OnTranslate(int diffX, int diffY)
	{		
		float diffXf = -0.02f * (float)diffX;
		float diffYf =  0.02f * (float)diffY;
		
		m_targetPos += Vfloat3(diffXf, diffYf, 0.0f) * m_quat;
	}
	
	void TrackballCamera::OnZoom(int diff)
	{
		float diffF = -0.015f * (float)diff;
		m_focus += Vfloat3(0.0f, 0.0f, diffF);
	}
} // namespace SI
