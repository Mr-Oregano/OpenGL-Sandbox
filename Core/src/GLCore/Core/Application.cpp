#include "glpch.h"
#include "Application.h"

#include "Log.h"
#include "Instrumentor.h"
#include "Input.h"

#include <glfw/glfw3.h>

namespace GLCore {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application *Application::s_Instance = nullptr;

	Application::Application(const std::string &name, WindowProps windowProps)
	{

		PROFILER_INIT();
		PROFILE_SESSION_BEGIN("Profile_Session-Startup");

		{

			PROFILE_FUNCTION();

			if (!s_Instance)
			{

				// Initialize core
				Log::Init();
			}

			GLCORE_ASSERT(!s_Instance, "Application already exists!");
			s_Instance = this;

			m_Window = std::unique_ptr<Window>(Window::Create(windowProps));
			m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

			// Renderer::Init();

		#if GLCORE_IMGUI
			m_ImGuiLayer = new ImGuiLayer();
			PushOverlay(m_ImGuiLayer);
		#endif

		}

		PROFILE_SESSION_END();

	}
	Application::~Application()
	{

		PROFILER_DEINIT();

	}

	void Application::PushLayer(Layer *layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer *layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	void Application::PopLayer(Layer *layer)
	{
		m_LayerStack.PopLayer(layer);
	}

	void Application::PopOverlay(Layer *layer)
	{
		m_LayerStack.PopOverlay(layer);
	}

	void Application::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{

		PROFILE_SESSION_BEGIN("Profile_Session-Runloop");

		{

			PROFILE_FUNCTION();
			while (m_Running)
			{

				PROFILE_SCOPE("Application Frame");
				float time = (float)glfwGetTime();
				Timestep timestep = time - m_LastFrameTime;
				m_LastFrameTime = time;

				{

					PROFILE_SCOPE("Layer OnUpdate");
					for (Layer *layer : m_LayerStack)
						layer->OnUpdate(timestep);

				}

				#if GLCORE_IMGUI
				{

					PROFILE_SCOPE("OnImGuiRender");
					m_ImGuiLayer->Begin();
					for (Layer *layer : m_LayerStack)
						layer->OnImGuiRender();
					m_ImGuiLayer->End();

				}
				#endif

				m_Window->OnUpdate();

			}

			// When we're done running, detach all layers so memory
			// can be cleaned up.
			for (Layer *layer : m_LayerStack)
				PopLayer(layer);
		}

		PROFILE_SESSION_END();
	}

	bool Application::OnWindowClose(WindowCloseEvent &e)
	{
		m_Running = false;
		return true;
	}

}
