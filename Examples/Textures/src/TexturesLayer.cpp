
#include "TexturesLayer.h"

#include <GLCore/Core/Input.h>
#include <GLCore/Core/Random.h>

#include <GLcore/Core/Instrumentor.h>

using namespace GLCore;
using namespace GLCore::Utils;

GLuint vao;
GLuint vbo;

TexturesLayer::TexturesLayer()
	: m_Camera(16.0f / 9.0f)
{
}

TexturesLayer::~TexturesLayer()
{
}

void TexturesLayer::OnAttach()
{
	EnableGLDebugging();
	
	m_LandTex = Texture2D::LoadFromFile("res/textures/land.jpg");
	m_SunsetTex = Texture2D::LoadFromFile("res/textures/sunset.jpg");

	m_Shader = Shader::FromGLSLTextFiles(
		"res/shaders/textures-vert.glsl",
		"res/shaders/textures-frag.glsl"
	);

	float positions[] = 
	{
		-0.5f, -0.5f,	0.0f, 0.0f,
		-0.5f,  0.5f,	0.0f, 1.0f,
		 0.5f,  0.5f,	1.0f, 1.0f,

		 0.5f,  0.5f,	1.0f, 1.0f,
		 0.5f, -0.5f,	1.0f, 0.0f,
		-0.5f, -0.5f,	0.0f, 0.0f
	};

	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void*) (sizeof(float) * 2));
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void TexturesLayer::OnDetach()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void TexturesLayer::OnEvent(Event& event)
{
	m_Camera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) 
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		return true;
	});
}

void TexturesLayer::OnUpdate(Timestep ts)
{
	m_Camera.OnUpdate(ts);
	
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vao);
	
	GLuint program = m_Shader->GetRendererID();
	glUseProgram(program);
	
	GLint loc = glGetUniformLocation(program, "u_ViewProjection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m_Camera.GetCamera().GetViewProjectionMatrix()));

	loc = glGetUniformLocation(program, "u_Model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4{1.0f}, { -0.5f, 0.0f, 0.0f })));
	m_LandTex->Bind(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4{1.0f}, { 0.5f, 0.0f, 0.0f })));
	m_SunsetTex->Bind(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void TexturesLayer::OnImGuiRender()
{
}

