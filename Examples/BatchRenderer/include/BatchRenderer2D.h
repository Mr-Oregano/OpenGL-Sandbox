#pragma once

#include <memory>

#include <GLCore.h>
#include <GLCoreUtils.h>

// TODO: Add support for texture rendering. Support for both texture units and texture atlases
//
struct Batch
{
	struct Vertex 
	{
		glm::vec4 color;
		glm::vec4 position;
		float rotation;
		float texture;
		glm::vec2 texelSize;
	};

	struct Material
	{
		GLuint textureHandle;
		int texWidth;
		int texHeight;
	};

	std::vector<Vertex> submissions;
	std::vector<Material> materials;

	unsigned int submission_count = 0;
	unsigned int material_count = 0;

	Batch(unsigned int vertexCap, unsigned int textureCap)
	{
		submissions.resize(vertexCap);
		materials.resize(textureCap);
	}
};

class BatchRenderer2D
{

public:
	BatchRenderer2D() = delete;
	~BatchRenderer2D() = delete;

	// The following functions require an OpenGL context to be present on the
	// calling thread.
	//
	static void Init(unsigned int capacity = 10000);
	static void Shutdown();

	static void Begin(
		const GLCore::Utils::OrthographicCamera &camera, 
		const glm::mat4 &transform = glm::mat4(1.0), 
		const glm::vec4 &tint = { 1.0f, 1.0f, 1.0f, 1.0f });

	static void End();

	static void DrawRect(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color, float rotation = 0);
	static void DrawRect(const glm::vec2 &pos, const glm::vec2 &size, Batch::Material material, float rotation = 0);
	static void DrawRect(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color, Batch::Material material, float rotation = 0);

	inline static unsigned int GetCapacity() { return s_Capacity; }
	static void SetCapacity(unsigned int capacity);

	// Debugging
	static void OnImGuiRender(bool *show);
	//

private:
	// Flushes the contents of the buffer data to the bound FBO.
	//    This method is internal to the class, it will be called when necessary
	//
	static void Flush();

private:
	struct Assets 
	{
		GLCore::Ref<GLCore::Utils::Shader> shader;

		GLuint vertexArray;
		GLuint vertexBuffer;

		Batch::Material white;
	};

private:
	static bool s_Initialized;

	static unsigned int s_Capacity;
	static unsigned int s_TextureSlots;
		
	static std::vector<Batch> s_Batches;
	static Assets *s_Assets;

public:
	// DEBUG functionality only
	static unsigned int s_FlushCount;
	//

};
