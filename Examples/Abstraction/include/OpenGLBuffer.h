#pragma once

#include <GLCore.h>

enum class AttributeType
{

	Float = 0,
	Float2,
	Float3,
	Float4,

	Int,
	Int2,
	Int3,
	Int4

};

struct AttributeTypeInfo
{
	unsigned int components; // Component count
	unsigned int size; // Total size in bytes
	unsigned int sizePerComponent; // Size of each component
	GLenum openglType;
	
	static AttributeTypeInfo GetInfo(AttributeType type);
};

class BufferLayout
{
public:
	struct Attribute
	{
		AttributeType type;
		const char* name;

		unsigned int stride = 0;
		const void* offset = 0;

		Attribute(AttributeType type, const char* name)
			: type(type), name(name) {}
	};

public:
	BufferLayout(std::initializer_list<Attribute> attributes);

	inline const std::vector<Attribute>& GetAttributeList() const { return m_Attributes; }
	inline unsigned int GetAttributeCount() const { return (unsigned int) m_Attributes.size(); }
	inline unsigned int GetStridePerVertex() const { return m_StridePerVertex; }

private:
	std::vector<Attribute> m_Attributes;
	unsigned int m_StridePerVertex = 0;

private:
	void CalculateStridesAndOffsets();

};

class VertexBuffer
{

public:
	VertexBuffer(float *data, unsigned int size, const BufferLayout &layout, GLenum usage = GL_STATIC_DRAW);
	~VertexBuffer();

	inline const BufferLayout& GetLayout() const { return m_Layout; }
	inline unsigned int GetVertexCount() const { return m_VertexCount; }
	inline void SubmitData(float *data, unsigned int size, unsigned int offset = 0) 
	{ 
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	}

	inline void Bind() const 
	{ 
		glBindBuffer(GL_ARRAY_BUFFER, m_ContextID); 
		m_IsBound = true; 
	}
	inline void Unbind() const 
	{ 	
		if (!m_IsBound)
			return; 
		glBindBuffer(GL_ARRAY_BUFFER, 0); 
		m_IsBound = false; 
	}

	static GLCore::Ref<VertexBuffer> Create(float *data, unsigned int size, const BufferLayout &layout, GLenum usage = GL_STATIC_DRAW);

private:
	unsigned int m_ContextID = 0;
	mutable bool m_IsBound = false;

	unsigned int m_VertexCount = 0;

	BufferLayout m_Layout;

};

class ElementBuffer 
{
public:
	ElementBuffer(unsigned int *data, unsigned int count);
	~ElementBuffer();

	inline unsigned int GetElementCount() const { return m_Count; }
	inline unsigned int GetContextID() const { return m_ContextID; }
	inline void SubmitData(unsigned int *data, unsigned int size, unsigned int offset = 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_ContextID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	static GLCore::Ref<ElementBuffer> Create(unsigned int *data, unsigned int count);
	
private:
	unsigned int m_ContextID = 0;
	unsigned int m_Count = 0;

};
