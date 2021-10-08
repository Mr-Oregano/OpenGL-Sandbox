
#include "Render2DQueue.h"

#include <string>

Render2DQueue::Render2DQueue(Render2DQueueProps props)
{
	m_VertexBufferStoreSize = props.batchSize;

	glCreateBuffers(1, &m_VertexBufferHandle);
	glNamedBufferStorage(m_VertexBufferHandle, m_VertexBufferStoreSize * sizeof(Vertex), nullptr, GL_MAP_WRITE_BIT);

	m_VertexBufferLocalStore = new Vertex[m_VertexBufferStoreSize];

	glCreateVertexArrays(1, &m_VertexArrayHandle);
	glVertexArrayVertexBuffer(m_VertexArrayHandle, 0, m_VertexBufferHandle, 0, sizeof(Vertex));
	
	glVertexArrayAttribFormat(m_VertexArrayHandle, 0, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(m_VertexArrayHandle, 1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4u);

	glVertexArrayAttribBinding(m_VertexArrayHandle, 0, 0);
	glVertexArrayAttribBinding(m_VertexArrayHandle, 1, 0);

	glEnableVertexArrayAttrib(m_VertexArrayHandle, 0);
	glEnableVertexArrayAttrib(m_VertexArrayHandle, 1);
}

Render2DQueue::~Render2DQueue()
{
	glDeleteBuffers(1, &m_VertexBufferHandle);
	m_VertexBufferHandle = 0;

	delete[] m_VertexBufferLocalStore;
	m_VertexBufferLocalStore = nullptr;
}

void Render2DQueue::Submit(RectTransform transform, Material material)
{
	if (m_SubmissionCount == m_VertexBufferStoreSize)
		Flush();

	// TODO: Deal with different materials create different batch groups if necessary
	//
	m_RenderCommands.material = material;

	// Get appropriate render command (if none found, create new)
	// 	   	
	auto it = m_TextureMap.find(material.textureHandle);
	unsigned int command;


	if (it == m_TextureMap.end())
	{
		++m_RenderCommandCount;
		command = m_RenderCommandCount - 1;

		m_RenderCommands.firstVertex[command] = m_SubmissionCount;
		m_RenderCommands.vertexCount[command] = 0;
		m_TextureMap.insert({ material.textureHandle, command });
	}
	else
		command = it->second;
	
	++m_RenderCommands.vertexCount[command];

	//

	Vertex &v = m_VertexBufferLocalStore[m_SubmissionCount];
	v.transform = transform;

	++m_SubmissionCount;
}

void Render2DQueue::Flush()
{
	glUseProgram(m_RenderCommands.material.shaderHandle);

	// TODO: Set material uniforms

	// Map the buffer data
	//
	size_t mapSize = m_SubmissionCount * sizeof(Vertex);
	void *data = glMapNamedBufferRange(m_VertexBufferHandle, 0, mapSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	
	std::memcpy(data, (const void*) m_VertexBufferLocalStore, mapSize);
	
	glUnmapNamedBuffer(m_VertexBufferHandle);
	data = nullptr;
	//

	glBindVertexArray(m_VertexArrayHandle);

	glMultiDrawArrays(GL_POINTS, m_RenderCommands.firstVertex, m_RenderCommands.vertexCount, m_RenderCommandCount);

	m_SubmissionCount = 0;
	m_RenderCommandCount = 0;
}
