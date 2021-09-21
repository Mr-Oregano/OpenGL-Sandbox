#include "GLCore.h"

#include "FontRenderingLayer.h"

using namespace GLCore;

class App : public Application
{
public:
	App()
		: Application("OpenGL Sandbox", {
			"OpenGL Sandbox",
			1280,
			720,
			true,
			Sampling::None,
			CursorMode::Default,
			ShowWindow::Maximized
		})
	{
		PushLayer(new FontRenderingLayer());
	}
};

int main()
{ 
	std::unique_ptr<App> app = std::make_unique<App>();
	app->Run();
}