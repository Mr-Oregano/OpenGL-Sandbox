#pragma once

#include "glpch.h"

#include "GLCore/Core/Core.h"
#include "GLCore/Events/Event.h"

namespace GLCore {

	enum class CursorMode
	{
		Default,
		Disabled
	};
	enum class Sampling
	{
		None = 0,
		Two = 2,
		Four = 4,
		Eight = 8,
		Sixteen = 16
	};
	enum class ShowWindow
	{
		Default = 0,
		Minimized,
		Maximized
	};

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		bool Vsync;
		Sampling Samples;
		CursorMode Cursor;

		ShowWindow Show;

		WindowProps(const std::string &title = "OpenGL Sandbox",
					uint32_t width = 1280,
					uint32_t height = 720,
					bool vsync = false,
					Sampling samples = Sampling::None,
					CursorMode cursor = CursorMode::Default,
					ShowWindow show = ShowWindow::Default)
			: Title(title), Width(width), Height(height), Vsync(vsync), Samples(samples), Cursor(cursor), Show(show)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event &)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn &callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetCursorMode(CursorMode cursor) = 0;
		virtual CursorMode GetCursorMode() = 0;

		virtual void *GetNativeWindow() const = 0;

		static Window *Create(const WindowProps &props = WindowProps());
	};

}
