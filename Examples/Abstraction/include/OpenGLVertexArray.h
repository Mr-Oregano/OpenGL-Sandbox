#pragma once

#include <GLCore.h>

#include "OpenGLBuffer.h"

class VertexArray 
{
public:
	VertexArray(GLenum primitive);
	~VertexArray();

	void AddBuffer(GLCore::Ref<VertexBuffer> buffer);
	void SetElementBuffer(GLCore::Ref<ElementBuffer> elementBuffer);

	inline const ElementBuffer& GetElementBuffer() const { return *m_ElementBuffer; }
	inline unsigned int GetVertexCount() const { return m_VertexCount; }
	inline GLenum GetPrimitiveType() const { return m_PrimitiveType; }

	static GLCore::Ref<VertexArray> Create(GLenum primitive = GL_TRIANGLES);

	inline void Bind() const 
	{ 
		glBindVertexArray(m_ContextID); 
		m_IsBound = true;
	}
	inline void Unbind() const
	{
		if (!m_IsBound)
			return;
		glBindVertexArray(0);
		m_IsBound = false;
	}

private:
	unsigned int m_ContextID = 0;
	unsigned int m_AttributeCount = 0;
	mutable bool m_IsBound = false;

	unsigned int m_VertexCount = 0;
	GLenum m_PrimitiveType = GL_TRIANGLES;

	GLCore::Ref<ElementBuffer> m_ElementBuffer = nullptr;
	std::vector<GLCore::Ref<VertexBuffer>> m_Buffers;

};
