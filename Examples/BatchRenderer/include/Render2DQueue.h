#pragma once

#include <Glad/glad.h>

#include <unordered_map>

struct RectTransform
{
	float x;
	float y;
	float w;
	float h;
	float rot;
};

struct Material
{
	GLuint shaderHandle;
	GLuint textureHandle;
};

struct Render2DQueueProps
{
	unsigned int batchSize;
};

struct Vertex
{
	RectTransform transform;
};

struct RenderCommandBuffer
{
	Material material{0};
	GLint firstVertex[16]{0};
	GLint vertexCount[16]{0};
};

class Render2DQueue
{
	// Render 2D queue. Submission machine that
	// will organize and batch submissions based on 
	// material, uniforms and other properties.

	//		All geometry is interally managed and assumed to be
	//		a QUAD (two triangles). Because of this a giant buffer
	//		of some customizable size will hold several submissions
	//		in an attempt to batch data being sent to the GPU.

	//		A submission involves a rect transform, and a material. 

	//		Rect Transform:
	//			x, y, w, h
	//			rotation

	//		Material:
	//			shader
	//			uniform-list*(1)
	//			textures & texcoords*(2)

	//		Geometry is sorted based on material. Materials with
	//		different shaders and uniforms will be grouped into a separate batch
	//		otherwise bindless textures will be used along with glMultiDraw...
	//		(where drawCount will be number of unique textures).
	//		All submissions with identical materials should be rendered in a single
	//		draw call, and any materials only differing in textures will make use
	//		of gl_DrawID in the shaders to retrieve a texture handle from some UBO array.
	//
	//		(1) Need to figure out how uniforms will work... should probably use UBO
	//		(2) Need to figure out how this will work with multiple textures.
	//

	//		Resources:
	//			- https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glMultiDrawElements.xhtml
	//			- https://www.khronos.org/opengl/wiki/Bindless_Texture
	//			- https://stackoverflow.com/a/40877255
	//			- https://www.khronos.org/opengl/wiki/Vertex_Rendering#Multi-Draw
	//			- https://gamedev.stackexchange.com/a/69845
	//

public:
	Render2DQueue(Render2DQueueProps props);
	~Render2DQueue();

	void Submit(RectTransform transform, Material material);
	void Flush();

private:
	std::unordered_map<GLuint, int> m_TextureMap;

	RenderCommandBuffer m_RenderCommands;
	unsigned int m_RenderCommandCount = 0;

	GLuint m_VertexBufferHandle = 0;
	Vertex *m_VertexBufferLocalStore = nullptr;
	unsigned int m_SubmissionCount = 0;
	unsigned int m_VertexBufferStoreSize = 0;

	GLuint m_VertexArrayHandle = 0;
};
