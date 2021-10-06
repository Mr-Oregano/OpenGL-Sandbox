 
#include "NoiseLayer.h"

#include <GLCore/Core/Input.h>
#include <GLCore/Core/Random.h>
#include <GLcore/Core/Instrumentor.h>

using namespace GLCore;
using namespace GLCore::Utils;

GLuint vertexArrayHandle;
GLuint vertexBufferHandle;

glm::vec3 color = { 0.5f, 0.73f, 1.0f };

NoiseLayer::NoiseLayer()
	: m_Camera(16.0f / 9.0f)
{
}

NoiseLayer::~NoiseLayer()
{
}

void NoiseLayer::OnAttach()
{
	EnableGLDebugging();
	glClearColor(0.13f, 0.13f, 0.15f, 1.0f);

	m_Shader = Shader::FromGLSLTextFiles(
		"res/generic-vert.glsl",
		"res/plasma3D-frag.glsl"
	);

	glUseProgram(m_Shader->GetRendererID());
	GLint loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Gradients");

	float gradients[] =
	{
		 1,  1,  0,   -1,  1,  0,    1, -1,  0,
		-1, -1,  0,    1,  0,  1,   -1,  0,  1,
		 1,  0, -1,   -1,  0, -1,    0,  1,  1,
		 0, -1,  1,    0,  1, -1,    0, -1, -1
	};

	glUniform1fv(loc, sizeof(gradients) / sizeof(float), gradients);

	glUseProgram(m_Shader->GetRendererID());
	loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");
	glUniform3fv(loc, 1, glm::value_ptr(color));

	float vertices[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f
	};

	glCreateVertexArrays(1, &vertexArrayHandle);
	glBindVertexArray(vertexArrayHandle);

	glCreateBuffers(1, &vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void NoiseLayer::OnDetach()
{
	glDeleteVertexArrays(1, &vertexArrayHandle);
	glDeleteBuffers(1, &vertexBufferHandle);
}

void NoiseLayer::OnEvent(Event &event)
{
	m_Camera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e)
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		return true;
	});
}

void NoiseLayer::OnUpdate(Timestep ts)
{
	static float time = 0;
	
	m_Camera.OnUpdate(ts);
	glClear(GL_COLOR_BUFFER_BIT);
	time += ts;

	GLint loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Time");
	glUniform1f(loc, time);
	
	glBindVertexArray(vertexArrayHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void NoiseLayer::OnImGuiRender()
{
	if (ImGui::ColorPicker3("Color", glm::value_ptr(color)))
	{
		GLint loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");
		glUniform3fv(loc, 1, glm::value_ptr(color));
	}
}