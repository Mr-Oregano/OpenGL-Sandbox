
#include "UniformBuffersLayer.h"

#include <ctime>

using namespace GLCore;
using namespace GLCore::Utils;

GLuint vertexArrayHandle;
GLuint vertexBufferHandle;
GLuint uniformBufferHandle;

UniformBuffersLayer::UniformBuffersLayer()
{
}

UniformBuffersLayer::~UniformBuffersLayer()
{
}

void UniformBuffersLayer::OnAttach()
{
	EnableGLDebugging();

	m_Shader = Shader::FromGLSLTextFiles(
		"res/ubo-vert.glsl",
		"res/ubo-frag.glsl"
	);
	
	GLuint program = m_Shader->GetRendererID();
	glUseProgram(program);

	float uniforms[] = 
	{
		0.0f, 0.0f, 1.0f, 1.0f
	};

	glCreateBuffers(1, &uniformBufferHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferHandle);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms), uniforms, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBufferHandle);

	float vertices[] =
	{
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		-0.5f,  0.5f,
		 0.5f,  0.5f
	};

	glCreateVertexArrays(1, &vertexArrayHandle);
	glBindVertexArray(vertexArrayHandle);

	glCreateBuffers(1, &vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void UniformBuffersLayer::OnDetach()
{
	glDeleteVertexArrays(1, &vertexArrayHandle);
	glDeleteBuffers(1, &vertexBufferHandle);
}

void UniformBuffersLayer::OnEvent(Event &event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) 
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		return true;
	});
}

void UniformBuffersLayer::OnUpdate(Timestep ts)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vertexArrayHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void UniformBuffersLayer::OnImGuiRender()
{
}