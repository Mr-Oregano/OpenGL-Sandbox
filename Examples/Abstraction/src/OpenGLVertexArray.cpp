
#include "OpenGLVertexArray.h"

VertexArray::VertexArray(GLenum primitive)
	: m_PrimitiveType(primitive)
{
	glCreateVertexArrays(1, &m_ContextID);
}
VertexArray::~VertexArray()
{
	LOG_TRACE("Deleting VertexArray - ContextID: {0}", m_ContextID);
	glDeleteVertexArrays(1, &m_ContextID);
}

void VertexArray::AddBuffer(GLCore::Ref<VertexBuffer> buffer)
{
	glBindVertexArray(m_ContextID);
	buffer->Bind();

	for (const auto &attribute : buffer->GetLayout().GetAttributeList())
	{
		AttributeTypeInfo typeInfo = AttributeTypeInfo::GetInfo(attribute.type);

		glVertexAttribPointer(m_AttributeCount, typeInfo.components, typeInfo.openglType, GL_FALSE, attribute.stride, attribute.offset);
		glEnableVertexAttribArray(m_AttributeCount);
		++m_AttributeCount;
	}

	buffer->Unbind();
	glBindVertexArray(0);

	m_VertexCount += buffer->GetVertexCount();
	m_Buffers.push_back(buffer);

}
void VertexArray::SetElementBuffer(GLCore::Ref<ElementBuffer> elementBuffer)
{
		
	glBindVertexArray(m_ContextID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer->GetContextID());
	glBindVertexArray(0);

	m_ElementBuffer = elementBuffer;
}
GLCore::Ref<VertexArray> VertexArray::Create(GLenum primitive)
{
	return GLCore::CreateRef<VertexArray>(primitive);
}
