
#include "FontRenderingLayer.h"
#include "FontRenderer.h"

using namespace GLCore;
using namespace GLCore::Utils;

FontRenderingLayer::FontRenderingLayer()
	: m_Camera(16.0f / 9.0f, true)
{
}

FontRenderingLayer::~FontRenderingLayer()
{
}

void FontRenderingLayer::OnAttach()
{
	EnableGLDebugging();	

	m_Anglodavek = Font::LoadFromFile("res/fonts/anglodavek.fnt");
	m_Roboto = Font::LoadFromFile("res/fonts/roboto.fnt");
	FontRenderer::Init();
}

void FontRenderingLayer::OnDetach()
{
	FontRenderer::Shutdown();
}

void FontRenderingLayer::OnEvent(Event& event)
{
	m_Camera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) 
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		return true;
	});
}

void FontRenderingLayer::OnUpdate(Timestep ts)
{
	m_Camera.OnUpdate(ts);
	glClear(GL_COLOR_BUFFER_BIT);

	FontRenderer::Begin(m_Camera.GetCamera());

	static float time = 0.0f;
	time += ts;

	float scale =  0.5f * glm::sin(glm::radians(time * 100.0f)) + 0.5f;

	FontRenderer::DrawString("Hello, world!", { 0, 0 }, 1.0f);
	time += ts;

	FontRenderer::End();
}

void FontRenderingLayer::OnImGuiRender()
{



}

