#pragma once

#include <GLCore.h>

#include "OpenGLVertexArray.h"

class RenderCommand
{

public:
	static inline void SetClearColor(const glm::vec4& color) { glClearColor(color.r, color.g, color.b, color.a); }
	static inline void Clear(GLenum flag = GL_COLOR_BUFFER_BIT) { glClear(flag); }

	static inline void Viewport(glm::ivec4 dimensions) { glViewport(dimensions.x, dimensions.y, dimensions.z, dimensions.w); }

	static void DrawIndexed(const GLCore::Ref<VertexArray> &vao);
	static void DrawIndexed(const GLCore::Ref<VertexArray> &vao, unsigned int count);

	static void Draw(const GLCore::Ref<VertexArray> &vao);
	static void Draw(const GLCore::Ref<VertexArray> &vao, unsigned int count);

	static void DrawIndexed(const VertexArray& vao);
	static void DrawIndexed(const VertexArray& vao, unsigned int count);

	static void Draw(const VertexArray &vao);
	static void Draw(const VertexArray &vao, unsigned int count);

};
