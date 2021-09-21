
#include "EffectsLayer.h"

#include <GLCore/Core/Input.h>
#include <GLCore/Core/Random.h>
#include <GLcore/Core/Instrumentor.h>

#include <stb_image/stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

GLuint vertexArrayHandle;
GLuint vertexBufferHandle;
GLuint elementBufferHandle;

EffectsLayer::EffectsLayer()
	: m_Camera(16.0f / 9.0f)
{
}

EffectsLayer::~EffectsLayer()
{
}

void EffectsLayer::OnAttach()
{
	EnableGLDebugging();
	glClearColor(0.13f, 0.13f, 0.15f, 1.0f);

	m_Shader = Shader::FromGLSLTextFiles(
		"res/effects-vert.glsl", 
		"res/effects-frag.glsl"
	);
	
	glUseProgram(m_Shader->GetRendererID());
	GLint loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Resolution");
	glUniform2f(loc, Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

	float vertices[] = 
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f
	};

	unsigned int indices[] = 
	{
		0, 1, 2,
		2, 3, 1
	};
	
	glCreateVertexArrays(1, &vertexArrayHandle);
	glBindVertexArray(vertexArrayHandle);
	
	glCreateBuffers(1, &vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glCreateBuffers(1, &elementBufferHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}
void EffectsLayer::OnDetach()
{
	glDeleteVertexArrays(1, &vertexArrayHandle);
	glDeleteBuffers(1, &vertexBufferHandle);
	glDeleteBuffers(1, &elementBufferHandle);
}

void EffectsLayer::OnEvent(Event& event)
{
	m_Camera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) 
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		GLint loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Resolution");
		glUniform2f(loc, e.GetWidth(), e.GetHeight());
		return true;
	});
}

void EffectsLayer::OnUpdate(Timestep ts)
{
	m_Camera.OnUpdate(ts);	
	glClear(GL_COLOR_BUFFER_BIT);
	static float u_Time = 0;
	u_Time += ts;

	auto[x, y] = Input::GetMousePosition();
	float width = (float) Application::Get().GetWindow().GetWidth();
	float height = (float) Application::Get().GetWindow().GetHeight();

	// m_Shader->SetUniformFloat2("u_Mouse", { x / width, y / height });
	// m_Shader->SetUniformFloat("u_Time" ,u_Time);
	glBindVertexArray(vertexArrayHandle);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}
void EffectsLayer::OnImGuiRender()
{
}

