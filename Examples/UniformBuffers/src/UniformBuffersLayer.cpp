
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
		0.5f, 0.3f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	glCreateBuffers(1, &uniformBufferHandle);
	glNamedBufferStorage(uniformBufferHandle, sizeof(uniforms), uniforms, 0);
	
	std::cout << "Block index: " << glGetUniformBlockIndex(program, "Uniforms") << std::endl;

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBufferHandle);

	// NOTE: In OpenGL 4.2+ it is possible to set bind the interface block 
	//		 directly in GLSL via the "binding" layout attribute.
	// 
	// glUniformBlockBinding(program, 0, 1);

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