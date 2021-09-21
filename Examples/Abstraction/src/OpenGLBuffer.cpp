
#include "OpenGLBuffer.h"

AttributeTypeInfo AttributeTypeInfo::GetInfo(AttributeType type) {
	switch (type)
	{
	case AttributeType::Float: return { 1, 4, 4, GL_FLOAT };
	case AttributeType::Int: return { 1, 4, 4, GL_INT };

	case AttributeType::Float2: return { 2, 8, 4, GL_FLOAT };
	case AttributeType::Int2: return { 2, 8, 4, GL_INT };

	case AttributeType::Float3: return { 3, 12, 4, GL_FLOAT };
	case AttributeType::Int3: return { 3, 12, 4, GL_INT };

	case AttributeType::Float4: return { 4, 16, 4, GL_FLOAT };
	case AttributeType::Int4: return { 4, 16, 4, GL_INT };

	default: LOG_WARN("Unknown attribute type: {0}", type); return { 0, 0, 0, 0 };
	}
}

BufferLayout::BufferLayout(std::initializer_list<Attribute> attributes)
	: m_Attributes(attributes)
{
	CalculateStridesAndOffsets();
}
void BufferLayout::CalculateStridesAndOffsets()
{
	size_t offset = 0;
	unsigned int stride = 0;

	// Calculate stride first, it must be known ahead of time
	// before setting it for each attribute.
	for (const auto &attribute : m_Attributes)
		stride += AttributeTypeInfo::GetInfo(attribute.type).size;

	m_StridePerVertex = stride;
	for (auto &attribute : m_Attributes)
	{
		attribute.stride = stride;
		attribute.offset = (const void*) offset;
		offset += AttributeTypeInfo::GetInfo(attribute.type).size;
	}
}

GLCore::Ref<VertexBuffer> VertexBuffer::Create(float *data, unsigned int size, const BufferLayout &layout, GLenum usage)
{
	return GLCore::CreateRef<VertexBuffer>(data, size, layout, usage);
}

VertexBuffer::VertexBuffer(float *data, unsigned int size, const BufferLayout& layout, GLenum usage)
	: m_Layout(layout)
{

	glCreateBuffers(1, &m_ContextID);
	glBindBuffer(GL_ARRAY_BUFFER, m_ContextID);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);

	m_VertexCount = size / layout.GetStridePerVertex();

	// Creation of a VertexBuffer shouldn't really leave anything bound
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{

	LOG_TRACE("Deleting VertexBuffer - ContextID: {0}", m_ContextID);
		
	Unbind();
		
	// Vertex Buffer won't actually get deleted until it is no longer
	// referenced by any VAOs, it will be flagged for deletion instead.
	glDeleteBuffers(1, &m_ContextID);

}

GLCore::Ref<ElementBuffer> ElementBuffer::Create(unsigned int *data, unsigned int count)
{
	return GLCore::CreateRef<ElementBuffer>(data, count);
}

ElementBuffer::ElementBuffer(unsigned int *data, unsigned int count)
	: m_Count(count)
{

	glCreateBuffers(1, &m_ContextID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ContextID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);

	// Creation of an ElementBuffer shouldn't really leave anything bound
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
ElementBuffer::~ElementBuffer()
{

	LOG_TRACE("Deleting ElementBuffer - ContextID: {0}", m_ContextID);

	m_Count = 0;

	// Element Buffer won't actually get deleted until it is no longer
	// referenced by any VAOs, it will be flagged for deletion instead.
	glDeleteBuffers(1, &m_ContextID);

}
