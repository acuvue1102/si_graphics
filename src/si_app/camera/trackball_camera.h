#pragma once

#include <cstdint>
#include "si_base/math/math.h"
#include "si_app/app/app_module.h"

namespace SI
{
	class TrackballCamera : public AppModule
	{
	public:
		TrackballCamera(int sortKey);
		~TrackballCamera();
		
		Vfloat4x4 GetView() const;

	protected:
		virtual void OnPreUpdate     (const App& app, const AppUpdateInfo&) override;
		
		virtual void OnMouseMove     (const App& app, int x, int y) override;
		virtual void OnMouseButton   (const App& app, MouseButton b, bool isDown) override;
		virtual void OnMouseWheel    (const App& app, int wheel) override;
		virtual void OnKeyboard      (const App& app, Key k, bool isDown) override;

	private:
		void OnRotate(int diffX, int diffY);
		void OnTranslate(int diffX, int diffY);
		void OnZoom(int diff);

	private:
		Vfloat4x4 m_view;
		Vquat     m_quat;
		Vfloat3   m_targetPos;
		Vfloat3   m_focus;
	};

} // namespace SI
