#pragma once

#include "GLCore/Core/Window.h"

#include <GLFW/glfw3.h>

namespace GLCore {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps &props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline uint32_t GetWidth() const override { return m_Data.props.Width; }
		inline uint32_t GetHeight() const override { return m_Data.props.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn &callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void SetCursorMode(CursorMode cursor) override;
		inline virtual CursorMode GetCursorMode() override { return m_Data.props.Cursor; }

		inline virtual void *GetNativeWindow() const { return m_Window; }
	private:
		virtual void Init(const WindowProps &props);
		virtual void Shutdown();
	private:
		GLFWwindow *m_Window;

		struct WindowData
		{
			WindowProps props;
			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}
