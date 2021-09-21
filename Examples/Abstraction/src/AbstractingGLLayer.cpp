
#include "AbstractingGLLayer.h"

#include <GLCore/Core/Input.h>
#include <GLCore/Core/Random.h>
#include <GLcore/Core/Instrumentor.h>

#include <stb_image/stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

AbstractingGLLayer::AbstractingGLLayer()
	: m_Camera(16.0f / 9.0f)
{
}
AbstractingGLLayer::~AbstractingGLLayer()
{
}

void AbstractingGLLayer::OnAttach()
{

	EnableGLDebugging();
	RenderCommand::SetClearColor({ 0.13f, 0.13f, 0.15f, 1.0f });

	m_Shader = Shader::FromGLSLTextFiles(
		"res/openglabstraction-vert.glsl", 
		"res/openglabstraction-frag.glsl"
	);
	glUseProgram(m_Shader->GetRendererID());

	float vertices[] = {
		-0.5f, -0.5f,	0.2f, 0.8f, 0.5f, 1.0f,
		 0.5f, -0.5f,	0.2f, 0.2f, 0.8f, 1.0f,
		-0.5f,  0.5f,	0.8f, 0.8f, 0.5f, 1.0f,
		 0.5f,  0.5f,	0.2f, 0.2f, 0.8f, 1.0f
	};
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 1
	};

	m_VertexArray = VertexArray::Create();
	m_VertexArray->AddBuffer(VertexBuffer::Create(vertices, sizeof(vertices), {
		{ AttributeType::Float2, "a_Position" },
		{ AttributeType::Float4, "a_Color"}
	}));
	m_VertexArray->SetElementBuffer(ElementBuffer::Create(indices, (unsigned int) std::size(indices)));

}
void AbstractingGLLayer::OnDetach()
{
}

void AbstractingGLLayer::OnEvent(Event& event)
{
	m_Camera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) {

		RenderCommand::Viewport({ 0, 0, e.GetWidth(), e.GetHeight() });
		return true;

	});
}

void AbstractingGLLayer::OnUpdate(Timestep ts)
{
	m_Camera.OnUpdate(ts);	
	RenderCommand::Clear();

	m_VertexArray->Bind();
	RenderCommand::DrawIndexed(m_VertexArray);
}

