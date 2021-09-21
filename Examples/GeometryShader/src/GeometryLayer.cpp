
#include "GeometryLayer.h"

using namespace GLCore;
using namespace GLCore::Utils;

GLuint vertexArrayHandle;
GLuint vertexBufferHandle;

GeometryLayer::GeometryLayer()
	: m_Camera(16.0f / 9.0f)
{
}

GeometryLayer::~GeometryLayer()
{
}

void GeometryLayer::OnAttach()
{
	EnableGLDebugging();
	
	m_Shader = Shader::FromGLSLTextFiles(
		"res/geomtest-vert.glsl",
		"res/geomtest-geom.glsl",
		"res/geomtest-frag.glsl"
	);
	
	glUseProgram(m_Shader->GetRendererID());
	GLint loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Side");
	glUniform1f(loc, 0.1f);

	float vertices[20 * 2] = { 0 };

	for (int i = 0; i < sizeof(vertices) / sizeof(float); i += 2)
	{
		vertices[i] = -0.9f + (i * 0.1f);
		vertices[i + 1] = 0.9f - (i * 0.1f);
	}

	glCreateVertexArrays(1, &vertexArrayHandle);
	glBindVertexArray(vertexArrayHandle);

	glCreateBuffers(1, &vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void GeometryLayer::OnDetach()
{
	glDeleteVertexArrays(1, &vertexArrayHandle);
	glDeleteBuffers(1, &vertexBufferHandle);
}

void GeometryLayer::OnEvent(Event& event)
{
	m_Camera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) 
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		return true;
	});
}

void GeometryLayer::OnUpdate(Timestep ts)
{
	m_Camera.OnUpdate(ts);

	glClear(GL_COLOR_BUFFER_BIT);

	GLint loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m_Camera.GetCamera().GetViewProjectionMatrix()));

	loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");
	glUniform4f(loc, 0.4f, 0.3f, 1.0f, 1.0f);

	glBindVertexArray(vertexArrayHandle);
	glDrawArrays(GL_POINTS, 0, 20);
}

void GeometryLayer::OnImGuiRender()
{
	// ImGui here
}
