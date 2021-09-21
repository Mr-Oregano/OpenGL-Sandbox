#include "GLCore.h"

#include "BatchRendererLayer.h"

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
		PushLayer(new BatchRendererLayer());
	}
};

int main()
{ 
	std::unique_ptr<App> app = std::make_unique<App>();
	app->Run();
}